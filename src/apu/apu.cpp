#include "nes/apu/apu.hpp"
#include <cmath>

namespace nes
{

    namespace
    {

        // Frequency (Hz) to NES 11-bit period: f = CPU/(16*(p+2)) => p = CPU/(16*f) - 2.
        constexpr u32 kCpuHz = 1789773;
        constexpr u32 kSampleRate = 44100;
        constexpr double kPi = 3.14159265358979323846;

        u16 noteToPeriod(u16 note)
        {
            // Free helper; member wrappers below.
            // note: 0 = A0 (27.5 Hz), one unit = one semitone.
            // f = 27.5 * 2^((note-21)/12) roughly; simpler: A4=69 => 440Hz.
            if (note > 127)
                note = 127;
            double freq = 440.0 * std::pow(2.0, (static_cast<int>(note) - 69) / 12.0);
            if (freq < 20.0)
                freq = 20.0;
            if (freq > 16000.0)
                freq = 16000.0;
            double p = static_cast<double>(kCpuHz) / (16.0 * freq) - 2.0;
            if (p < 0)
                p = 0;
            if (p > 2047)
                p = 2047;
            return static_cast<u16>(p);
        }

        u32 periodToPhaseInc(u16 period)
        {
            // resampled to sample rate: inc = 1024 * f / sampleRate.
            double freq = static_cast<double>(kCpuHz) / (16.0 * (period + 2));
            double inc = 1024.0 * freq / static_cast<double>(kSampleRate);
            if (inc < 1.0)
                inc = 1.0;
            if (inc > 100000.0)
                inc = 100000.0;
            return static_cast<u32>(inc);
        }

        // NES pulse duty waveform lookup (phases 0-7, 1 = high).
        constexpr u8 kPulseDuty[4][8] = {
            {0, 1, 0, 0, 0, 0, 0, 0}, // 12.5%
            {0, 1, 1, 0, 0, 0, 0, 0}, // 25%
            {0, 1, 1, 1, 1, 0, 0, 0}, // 50%
            {0, 0, 1, 1, 1, 1, 1, 1}, // 75%
        };

        // NES length counter load table, indexed by the 5-bit length field.
        constexpr u16 kLengthTable[32] = {
            10, 254, 20, 2, 40, 4, 80, 6, 160, 8, 60, 10, 14, 12, 26, 14,
            12, 16, 24, 18, 48, 20, 96, 22, 192, 24, 72, 26, 16, 28, 32, 30,
        };

        // Noise/DMC period index -> phase increment at the output sample rate.
        constexpr u32 kNoiseRateTable[16] = {
            4, 8, 16, 30, 60, 88, 118, 148, 188, 226, 262, 300, 344, 390, 440, 490,
        };

        float noiseSample(Voice &v)
        {
            // 15-bit LFSR, feedback bit 0 xor bit 1 (short mode: bit 6).
            const u8 bit = static_cast<u8>((v.noiseLfsr & 1u) ^ ((v.noiseLfsr >> 1) & 1u));
            v.noiseLfsr = static_cast<u16>((v.noiseLfsr >> 1) | (bit << 14));
            return (v.noiseLfsr & 1u) ? 1.0f : -1.0f;
        }

    } // namespace

    u8 APU::read(u16 address)
    {
        if (address == 0x4015)
            return static_cast<u8>((status_ & 0x1Fu) | (registers_[0x15] & 0xE0u));
        if (address >= 0x4000 && address <= 0x4017)
            return registers_[address - 0x4000];
        if (address >= 0x4018 && address <= 0x401F)
        {
            // Extended voice control space (house expansion): read voice regs back.
            const u8 index = static_cast<u8>(address - 0x4018);
            if (index < kVoiceCount)
            {
                const Voice &v = voices_[index];
                return static_cast<u8>((v.volume & 0x0Fu) | (v.duty << 4) |
                                       (v.enabled ? 0x80u : 0u));
            }
        }
        return 0;
    }

    void APU::write(u16 address, u8 value)
    {
        if (address < 0x4000 || address > 0x401F)
            return;
        if (address <= 0x4017)
        {
            registers_[address - 0x4000] = value;
            decodeChannelRegister(address, value);
            if (address == 0x4015)
            {
                status_ = static_cast<u8>(value & 0x1Fu);
                // Channel enables: bit 0-3 = pulse1, pulse2, triangle, noise.
                setVoiceEnabled(0, (value & 0x01u) != 0);
                setVoiceEnabled(1, (value & 0x02u) != 0);
                setVoiceEnabled(2, (value & 0x04u) != 0);
                setVoiceEnabled(3, (value & 0x08u) != 0);
                setVoiceEnabled(4, (value & 0x10u) != 0);
            }
            return;
        }

        // Extended $4018-$401F voice control (9 voices, 2 per write).
        const u8 index = static_cast<u8>((address - 0x4018) * 2);
        if (index >= kVoiceCount)
            return;
        setVoiceEnabled(index, (value & 0x01u) != 0);
        if (index + 1 < kVoiceCount)
            setVoiceEnabled(index + 1, (value & 0x02u) != 0);
    }

    // Decode $4000-$4013 channel registers into voice state.
    void APU::decodeChannelRegister(u16 address, u8 value)
    {
        if (address <= 0x4007)
        {
            // Pulse 1 ($4000-$4003) or Pulse 2 ($4004-$4007).
            const u8 voiceIndex = static_cast<u8>((address >= 0x4004) ? 1 : 0);
            Voice &v = voices_[voiceIndex];
            v.type = VoiceType::Pulse;
            switch (address & 0x0003u)
            {
            case 0x0: // $4000/$4004: duty, volume, envelope
                v.duty = static_cast<u8>((value >> 6) & 0x03u);
                v.volume = static_cast<u8>(value & 0x0Fu);
                v.envelopeLoop = (value & 0x20u) != 0;
                v.envelopeDisable = (value & 0x10u) != 0;
                v.envelopeStart = true;
                break;
            case 0x1: // $4001/$4005: sweep
                v.sweepEnabled = (value & 0x80u) != 0;
                v.sweepPeriod = static_cast<u8>((value >> 4) & 0x07u);
                v.sweepNegate = (value & 0x08u) != 0;
                v.sweepShift = static_cast<u8>(value & 0x07u);
                break;
            case 0x2: // $4002/$4006: period low
                v.period = static_cast<u16>((v.period & 0x0700u) | value);
                v.phaseInc = periodToPhaseInc(v.period);
                break;
            case 0x3: // $4003/$4007: length + period high
            {
                v.period = static_cast<u16>((v.period & 0x00FFu) | ((value & 0x07u) << 8));
                v.phaseInc = periodToPhaseInc(v.period);
                v.lengthCounter = kLengthTable[value >> 3];
                v.lengthEnabled = true;
                v.envelopeStart = true;
                break;
            }
            default:
                break;
            }
        }
        else if (address <= 0x400B)
        {
            // Triangle ($4008-$400B).
            Voice &v = voices_[2];
            v.type = VoiceType::Triangle;
            switch (address & 0x0003u)
            {
            case 0x0: // $4008: control flag + linear counter
                v.envelopeLoop = (value & 0x80u) != 0;
                v.envelopeCounter = static_cast<u8>(value & 0x7Fu);
                break;
            case 0x2: // $400A: period low
                v.period = static_cast<u16>((v.period & 0x0700u) | value);
                v.phaseInc = periodToPhaseInc(v.period);
                break;
            case 0x3: // $400B: length + period high
            {
                v.period = static_cast<u16>((v.period & 0x00FFu) | ((value & 0x07u) << 8));
                v.phaseInc = periodToPhaseInc(v.period);
                v.lengthCounter = kLengthTable[value >> 3];
                v.lengthEnabled = true;
                break;
            }
            default:
                break;
            }
        }
        else if (address <= 0x400F)
        {
            // Noise ($400C-$400F).
            Voice &v = voices_[3];
            v.type = VoiceType::Noise;
            switch (address & 0x0003u)
            {
            case 0x0: // $400C: volume/envelope
                v.volume = static_cast<u8>(value & 0x0Fu);
                v.envelopeLoop = (value & 0x20u) != 0;
                v.envelopeDisable = (value & 0x10u) != 0;
                v.envelopeStart = true;
                break;
            case 0x1: // $400D: unused
                break;
            case 0x2: // $400E: noise mode + period
                v.noiseShortMode = (value & 0x80u) != 0;
                v.period = static_cast<u16>(value & 0x0Fu);
                v.phaseInc = kNoiseRateTable[value & 0x0Fu];
                break;
            case 0x3: // $400F: length
                v.lengthCounter = kLengthTable[value >> 3];
                v.lengthEnabled = true;
                v.envelopeStart = true;
                break;
            }
        }
        else if (address <= 0x4013)
        {
            // DMC ($4010-$4013) -> Sample voice (voice 4).
            Voice &v = voices_[4];
            v.type = VoiceType::Sample;
            switch (address & 0x0003u)
            {
            case 0x0: // $4010: flags + rate
                v.sampleLoopEnabled = (value & 0x40u) != 0;
                v.sampleRate = static_cast<u8>(value & 0x0Fu);
                v.phaseInc = kNoiseRateTable[value & 0x0Fu];
                break;
            case 0x1: // $4011: direct load
                v.sampleOutput = static_cast<u8>(value & 0x7Fu);
                break;
            case 0x2: // $4012: sample address
                v.sampleAddress = static_cast<u16>(0xC000 + value * 64);
                break;
            case 0x3: // $4013: sample length
                v.sampleLength = static_cast<u16>(value * 16 + 1);
                break;
            }
        }
    }

    void APU::reset()
    {
        registers_.fill(0);
        status_ = 0;
        frameCounter_ = 0;
        frameCounterMode_ = 0;
        frameIrqInhibit_ = 0;
        frameStep_ = 0;
        frameDivider_ = 0;
        audioSampleCounter_ = 0;
        audioSampleAccumulator_ = 0;
        audioSampleLeft_ = 0.0f;
        audioSampleRight_ = 0.0f;
        audioBuffer_.clear();
        voices_.clear();
        voices_.resize(kVoiceCount);
        tracker_ = Tracker{};
        tracker_.patterns.clear();
    }

    void APU::clock()
    {
        clockTracker();

        // Advance synthesis at the output sample rate using a CPU-cycle accumulator.
        // APU master rate is CPU/2; we accumulate CPU cycles to emit samples.
        audioSampleAccumulator_ += kSampleRate * 2; // fixed point: CPU ticks per sample
        while (audioSampleAccumulator_ >= kCpuHz)
        {
            audioSampleAccumulator_ -= kCpuHz;
            float left = 0.0f;
            float right = 0.0f;
            for (u8 i = 0; i < kVoiceCount; ++i)
            {
                clockVoice(i);
                mixVoice(i, left, right);
            }
            audioSampleLeft_ = left;
            audioSampleRight_ = right;
            audioBuffer_.push_back(left);
            audioBuffer_.push_back(right);
        }
    }

    void APU::clockVoice(u8 i)
    {
        Voice &v = voices_[i];
        if (!v.enabled)
            return;

        // Sweep / pitch slide.
        if (v.sweepEnabled && v.sweepPeriod > 0)
        {
            if (++v.sweepCounter >= v.sweepPeriod)
            {
                v.sweepCounter = 0;
                v.period = static_cast<u16>(v.sweepNegate ? v.period - (v.period >> v.sweepShift)
                                                          : v.period + (v.period >> v.sweepShift));
                if (v.period > 2047)
                    v.period = 2047;
                v.phaseInc = periodToPhaseInc(v.period);
            }
        }
        if (v.slideRate > 0 && v.slideTarget != 0)
        {
            const u32 target = periodToPhaseInc(static_cast<u16>(v.slideTarget));
            u32 inc = periodToPhaseInc(v.period);
            if (inc < target)
            {
                inc = (inc + v.slideRate > target) ? target : inc + v.slideRate;
                v.period = static_cast<u16>(periodToPhaseInc(0) != 0 ? 0 : v.period);
            }
            else if (inc > target)
            {
                inc = (inc > v.slideRate) ? inc - v.slideRate : target;
                v.period = static_cast<u16>(periodToPhaseInc(0) != 0 ? 0 : v.period);
            }
            v.phaseInc = inc;
            if (v.phaseInc == target)
                v.slideRate = 0;
        }

        // Vibrato/tremolo LFO.
        if (v.lfoMode != 0 && v.lfoRate > 0)
        {
            v.lfoCounter += v.lfoRate;
            if (v.lfoCounter >= kSampleRate)
            {
                v.lfoCounter -= kSampleRate;
                v.lfoPhase2 += 1;
            }
            const int halfPeriod = 6; // ~5 Hz at 44.1kHz rate count
            const int phase = static_cast<int>(v.lfoPhase2 % 12);
            const int delta = (phase < halfPeriod) ? (v.lfoDepth * (halfPeriod - phase)) / halfPeriod
                                                   : -(v.lfoDepth * (phase - halfPeriod)) / halfPeriod;
            if (v.lfoMode == 1)
            { // vibrato
                v.phaseInc = periodToPhaseInc(v.period) + static_cast<u32>(delta);
            }
            else
            { // tremolo
                v.volume = static_cast<u8>((v.volumeEnvActive ? v.volumeEnvCurrent : v.volume) +
                                           (delta > 0 ? 0 : 0));
            }
        }

        // Envelope generator (volume decay).
        if (v.envelopeStart)
        {
            v.envelopeStart = false;
            v.envelopeDecay = 15;
            v.envelopeCounter = v.envelopeDivisor;
        }
        else if (v.envelopeDivisor > 0)
        {
            if (v.envelopeCounter == 0)
            {
                v.envelopeCounter = v.envelopeDivisor;
                if (v.envelopeDecay > 0)
                    --v.envelopeDecay;
                else if (v.envelopeLoop)
                    v.envelopeDecay = 15;
            }
            else
            {
                --v.envelopeCounter;
            }
        }

        // Length counter.
        if (v.lengthEnabled && v.lengthCounter > 0)
        {
            if (--v.lengthCounter == 0)
                v.enabled = false;
        }

        // Volume envelope from tracker.
        if (v.volumeEnvActive)
        {
            if (v.volumeEnvCounter >= v.volumeEnvRate)
            {
                v.volumeEnvCounter = 0;
                if (v.volumeEnvCurrent < v.volumeEnvTarget)
                    ++v.volumeEnvCurrent;
                else if (v.volumeEnvCurrent > v.volumeEnvTarget)
                    --v.volumeEnvCurrent;
                else
                    v.volumeEnvActive = false;
            }
            else
            {
                ++v.volumeEnvCounter;
            }
            v.volume = v.volumeEnvCurrent;
        }

        // Arpeggio.
        if (v.arpMode != 0)
        {
            if (++v.arpTimer >= v.arpSpeed)
            {
                v.arpTimer = 0;
                const u8 count = (v.arpNotes[2] != 0) ? 3 : ((v.arpNotes[1] != 0) ? 2 : 1);
                if (count > 1)
                {
                    v.arpIndex = (v.arpIndex + 1) % count;
                    const u16 arpNote = v.arpNotes[v.arpIndex];
                    v.phaseInc = periodToPhaseInc(static_cast<u16>(noteToPeriod(arpNote)));
                }
            }
        }

        // Sample playback (DMC-style).
        if (v.type == VoiceType::Sample)
        {
            if (v.sampleBytesRemaining == 0 && v.sampleLoopEnabled)
            {
                v.sampleBytesRemaining = v.sampleLength;
                v.sampleAddr = v.sampleAddress;
            }
            if (v.sampleBytesRemaining > 0)
            {
                // One bit per CPU cycle approximation.
                if (v.sampleBitsRemaining == 0)
                {
                    v.sampleCurrentByte = 0; // memory fetch is a stub until bus hooks exist
                    v.sampleBitsRemaining = 8;
                }
                const u8 bit = static_cast<u8>(v.sampleCurrentByte & 1u);
                v.sampleCurrentByte >>= 1;
                --v.sampleBitsRemaining;
                if (bit)
                {
                    if (v.sampleOutput <= 125)
                        v.sampleOutput += 2;
                }
                else
                {
                    if (v.sampleOutput >= 2)
                        v.sampleOutput -= 2;
                }
                if (v.sampleBitsRemaining == 0)
                    --v.sampleBytesRemaining;
            }
        }

        // Phase advance (pitch for tone voices, LFSR rate for noise).
        switch (v.type)
        {
        case VoiceType::Noise:
        {
            // LFSR clocked at CPU/(period+1); clock it from the phase accumulator.
            v.phase += v.phaseInc;
            while (v.phase >= 1024)
            {
                v.phase -= 1024;
                (void)noiseSample(v);
            }
            break;
        }
        case VoiceType::Sample:
            // handled above
            break;
        case VoiceType::Fm:
        {
            v.fmModPhaseInc = periodToPhaseInc(v.period) * (v.fmRatio + 1);
            v.fmModPhase += v.fmModPhaseInc;
            if (v.fmModPhase >= 1024)
                v.fmModPhase %= 1024;
            v.fmPhase += v.phaseInc;
            if (v.fmPhase >= 1024)
                v.fmPhase %= 1024;
            break;
        }
        default:
            v.phase += v.phaseInc;
            if (v.phase >= 1024)
                v.phase %= 1024;
            break;
        }
    }

    float APU::sampleVoice(u8 i)
    {
        Voice &v = voices_[i];
        if (!v.enabled || v.volume == 0)
            return 0.0f;

        float sample = 0.0f;
        switch (v.type)
        {
        case VoiceType::Pulse:
        {
            const u8 step = static_cast<u8>((v.phase >> 7) & 0x07u);
            sample = kPulseDuty[v.duty & 3u][step] ? 1.0f : 0.0f;
            break;
        }
        case VoiceType::Triangle:
        {
            const u8 step = static_cast<u8>((v.phase >> 7) & 0x1Fu); // 32 steps
            sample = (step < 16) ? (15.0f - step) / 7.5f - 1.0f
                                 : (step - 16) / 7.5f - 1.0f;
            break;
        }
        case VoiceType::Noise:
        {
            sample = (v.noiseLfsr & 1u) ? 1.0f : -1.0f;
            break;
        }
        case VoiceType::Sample:
        {
            sample = (static_cast<float>(v.sampleOutput) - 64.0f) / 64.0f;
            break;
        }
        case VoiceType::Fm:
        {
            const float mod = std::sin(2.0f * static_cast<float>(kPi) *
                                       static_cast<float>(v.fmModPhase) / 1024.0f);
            const float fmPhase =
                static_cast<float>(v.fmPhase) + mod * static_cast<float>(v.fmIndex) * 8.0f;
            sample = std::sin(2.0f * static_cast<float>(kPi) * fmPhase / 1024.0f);
            break;
        }
        }
        return sample * (static_cast<float>(v.volume) / 15.0f);
    }

    void APU::mixVoice(u8 i, float &left, float &right)
    {
        const float s = sampleVoice(i);
        if (s == 0.0f)
            return;
        const Voice &v = voices_[i];
        const float l = static_cast<float>(255 - v.pan) / 255.0f;
        const float r = static_cast<float>(v.pan) / 255.0f;
        left += s * l;
        right += s * r;
    }

    // ---------------- Tracker ----------------

    void APU::clockTracker()
    {
        if (!tracker_.playing || tracker_.patterns.empty())
            return;
        const TrackerPattern &pat = tracker_.patterns[tracker_.patternIndex];
        if (++tracker_.tickCounter < tracker_.speed)
            return;
        tracker_.tickCounter = 0;

        if (tracker_.rowIndex >= pat.rows)
        {
            tracker_.rowIndex = 0;
            tracker_.patternIndex = static_cast<u16>((tracker_.patternIndex + 1) % tracker_.patterns.size());
            return;
        }

        for (u8 v = 0; v < kVoiceCount && v < pat.voiceCount; ++v)
        {
            processTrackerCell(static_cast<u8>(v), pat.cell(static_cast<u8>(tracker_.rowIndex), v));
        }
        ++tracker_.rowIndex;
        if (tracker_.rowIndex >= pat.rows)
        {
            tracker_.rowIndex = 0;
            tracker_.patternIndex = static_cast<u16>((tracker_.patternIndex + 1) % tracker_.patterns.size());
        }
    }

    void APU::processTrackerCell(u8 i, const TrackerCell &cell)
    {
        Voice &v = voices_[i];
        if (cell.instrument != 0xFF && cell.instrument < 16)
            v.duty = static_cast<u8>(cell.instrument & 3u);

        if (cell.volume != 0xFF)
        {
            v.volume = static_cast<u8>(cell.volume & 0x0Fu);
            v.volumeEnvActive = false;
        }

        if (cell.note != 0xFFFF)
        {
            // Note on: set pitch, restart envelope and length.
            const u16 note = (cell.note <= 127) ? cell.note : 60;
            v.period = static_cast<u16>(noteToPeriod(note));
            v.basePeriod = v.period;
            v.phaseInc = periodToPhaseInc(v.period);
            v.slideRate = 0;
            v.slideTarget = 0;
            v.arpMode = 0;
            v.arpNotes[0] = v.arpNotes[1] = v.arpNotes[2] = 0;
            v.lfoMode = 0;
            v.lfoDepth = 0;
            if (!v.enabled && cell.volume != 0)
            {
                v.enabled = true;
                v.envelopeStart = true;
            }
        }

        if (cell.effect != 0xFF && cell.effect != static_cast<u8>(TrackerEffect::None))
        {
            triggerVoiceMod(i, static_cast<TrackerEffect>(cell.effect & 0x0Fu), cell.effectParam);
        }
    }

    void APU::triggerVoiceMod(u8 i, TrackerEffect effect, u8 param)
    {
        if (i >= kVoiceCount)
            return;
        Voice &v = voices_[i];
        switch (effect)
        {
        case TrackerEffect::Arpeggio:
        {
            v.arpMode = 1;
            v.arpNotes[0] = static_cast<u16>(param);
            v.arpNotes[1] = static_cast<u16>(param + 4);
            v.arpNotes[2] = static_cast<u16>(param + 7);
            v.arpIndex = 0;
            v.arpSpeed = 1;
            break;
        }
        case TrackerEffect::Vibrato:
            v.lfoMode = 1;
            v.lfoDepth = static_cast<u8>(param & 0x0Fu);
            v.lfoRate = static_cast<u8>((param >> 4) & 0x0Fu);
            break;
        case TrackerEffect::Tremolo:
            v.lfoMode = 2;
            v.lfoDepth = static_cast<u8>(param & 0x0Fu);
            v.lfoRate = static_cast<u8>((param >> 4) & 0x0Fu);
            break;
        case TrackerEffect::Portamento:
        {
            v.slideTarget = static_cast<u32>(param);
            v.slideRate = 1 + (param >> 4);
            break;
        }
        case TrackerEffect::VolumeEnvelope:
            v.volumeEnvActive = true;
            v.volumeEnvTarget = static_cast<u8>(param & 0x0Fu);
            v.volumeEnvRate = static_cast<u8>((param >> 4) & 0x0Fu);
            v.volumeEnvCurrent = v.volume;
            break;
        case TrackerEffect::DutyCycle:
            v.duty = static_cast<u8>(param & 3u);
            break;
        case TrackerEffect::NoiseAttack:
            v.envelopeLoop = false;
            v.envelopeDivisor = static_cast<u8>(param & 0x0Fu);
            v.envelopeStart = true;
            break;
        case TrackerEffect::Pan:
            v.pan = param;
            break;
        case TrackerEffect::SampleOffset:
            v.sampleAddr = static_cast<u16>(v.sampleAddress + param);
            v.sampleBytesRemaining = v.sampleLength;
            break;
        case TrackerEffect::ResetSweep:
            v.sweepEnabled = false;
            v.sweepPeriod = 0;
            v.slideRate = 0;
            v.slideTarget = 0;
            break;
        case TrackerEffect::JumpToRow:
            tracker_.rowIndex = param < tracker_.patterns[tracker_.patternIndex].rows
                                    ? param
                                    : tracker_.patterns[tracker_.patternIndex].rows;
            break;
        case TrackerEffect::SetVolume:
            v.volume = static_cast<u8>(param & 0x0Fu);
            break;
        case TrackerEffect::PatternBreak:
            tracker_.rowIndex = tracker_.patterns[tracker_.patternIndex].rows;
            break;
        case TrackerEffect::EndSong:
            tracker_.playing = false;
            for (auto &voice : voices_)
                voice.enabled = false;
            break;
        case TrackerEffect::SetSpeed:
            tracker_.speed = param < 1 ? 1 : param;
            break;
        case TrackerEffect::None:
        default:
            break;
        }
    }

    // ---------------- Direct host API ----------------

    void APU::setVoiceEnabled(u8 i, bool on)
    {
        if (i >= kVoiceCount)
            return;
        voices_[i].enabled = on;
        if (on)
            voices_[i].envelopeStart = true;
    }
    void APU::setVoiceType(u8 i, VoiceType type)
    {
        if (i >= kVoiceCount)
            return;
        voices_[i].type = type;
    }
    void APU::setVoicePeriod(u8 i, u16 period)
    {
        if (i >= kVoiceCount)
            return;
        voices_[i].period = static_cast<u16>(period > 2047 ? 2047 : period);
        voices_[i].basePeriod = voices_[i].period;
        voices_[i].phaseInc = periodToPhaseInc(voices_[i].period);
    }
    void APU::setVoiceVolume(u8 i, u8 volume)
    {
        if (i >= kVoiceCount)
            return;
        voices_[i].volume = static_cast<u8>(volume & 0x0Fu);
        voices_[i].volumeEnvActive = false;
    }
    void APU::setVoiceDuty(u8 i, u8 duty)
    {
        if (i >= kVoiceCount)
            return;
        voices_[i].duty = static_cast<u8>(duty & 3u);
    }
    void APU::setVoicePan(u8 i, u8 pan)
    {
        if (i >= kVoiceCount)
            return;
        voices_[i].pan = pan;
    }

    u32 APU::periodToPhaseInc(u16 period) const
    {
        return nes::periodToPhaseInc(period);
    }
    u32 APU::noteToPeriod(u16 note) const
    {
        return nes::noteToPeriod(note);
    }

}
