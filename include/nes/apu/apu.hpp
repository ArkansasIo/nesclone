#pragma once
#include "nes/types.hpp"
#include "nes/audio/audio.hpp"
#include <array>
#include <vector>

namespace nes
{

    // Voice types for the 9-channel APU.
    enum class VoiceType : u8
    {
        Pulse = 0,    // 0,1: standard NES pulse (duty 12.5/25/50/75)
        Triangle = 2, // 2: triangle
        Noise = 3,    // 3: noise LFSR
        Sample = 4,   // 4: DMC sample playback
        Fm = 5,       // 5-8: expansion 2-op FM voices
    };

    // Mod tracker effects.
    enum class TrackerEffect : u8
    {
        None = 0x00,
        Arpeggio = 0x01,       // cycles through notes
        Vibrato = 0x02,        // frequency LFO
        Portamento = 0x03,     // slide to target note
        Tremolo = 0x04,        // volume LFO
        VolumeEnvelope = 0x05, // attack + decay envelope
        DutyCycle = 0x06,      // change pulse duty
        NoiseAttack = 0x07,    // noise envelope attack
        Pan = 0x08,            // stereo position (0-255)
        SampleOffset = 0x09,   // DMC sample offset
        ResetSweep = 0x0A,     // clear pitch slide
        JumpToRow = 0x0B,      // jump to pattern row
        SetVolume = 0x0C,      // set volume directly
        PatternBreak = 0x0D,   // end pattern
        EndSong = 0x0E,        // stop tracker
        SetSpeed = 0x0F,       // ticks per row
    };

    // One mod tracker cell: a single voice's column in a pattern row.
    struct TrackerCell
    {
        u16 note = 0xFFFF;    // 0xFFFF = keep current note; 0-5118 = period value
        u8 instrument = 0xFF; // 0xFF = keep current
        u8 volume = 0xFF;     // 0xFF = keep current
        u8 effect = 0xFF;     // 0xFF = none
        u8 effectParam = 0;
    };

    // A mod tracker pattern: rows x voices grid of cells.
    struct TrackerPattern
    {
        u8 rows = 64;
        u8 voiceCount = 9;
        std::vector<TrackerCell> cells;
        TrackerPattern() { cells.assign(rows * voiceCount, TrackerCell{}); }
        TrackerPattern(u8 numRows, u8 numVoices) : rows(numRows), voiceCount(numVoices)
        {
            cells.assign(rows * voiceCount, TrackerCell{});
        }
        [[nodiscard]] TrackerCell &cell(u8 row, u8 voice) { return cells[row * voiceCount + voice]; }
        [[nodiscard]] const TrackerCell &cell(u8 row, u8 voice) const { return cells[row * voiceCount + voice]; }
    };

    // The tracker sequencer state.
    struct Tracker
    {
        bool playing = false;
        u16 patternIndex = 0;
        u16 rowIndex = 0;
        u8 speed = 6; // CPU frames per row (higher = slower)
        u8 tickCounter = 0;
        std::vector<TrackerPattern> patterns;
    };

    // Per-voice synthesis state.
    struct Voice
    {
        bool enabled = false;
        VoiceType type = VoiceType::Pulse;
        u16 period = 0;     // 11-bit period (NES style)
        u16 basePeriod = 0; // for pitch slides
        u32 phase = 0;      // 0-1023 phase accumulator (per-sample fixed point)
        u32 phaseInc = 0;   // per-sample phase increment
        u8 duty = 2;        // 0-3 duty cycle
        u8 volume = 15;     // 0-15
        u8 pan = 128;       // 0=left, 128=center, 255=right

        // Noise
        u16 noiseLfsr = 1;
        bool noiseShortMode = false;

        // Envelope (length counter)
        u16 lengthCounter = 0;
        bool lengthEnabled = false;
        bool envelopeLoop = false;
        bool envelopeDisable = false;
        u8 envelopeDivisor = 0;
        u8 envelopeDecay = 15;
        u8 envelopeCounter = 0;
        bool envelopeStart = false;

        // Pitch sweep
        bool sweepEnabled = false;
        u8 sweepPeriod = 0;
        u8 sweepShift = 0;
        bool sweepNegate = false;
        u8 sweepCounter = 0;
        u32 sweepTarget = 0;
        u32 sweepRate = 0;
        u32 sweepAccumulator = 0;

        // DMC / sample
        u16 sampleAddress = 0;
        u16 sampleLength = 0;
        u16 sampleBitsRemaining = 0;
        u8 sampleShift = 0;
        u8 sampleCurrentByte = 0;
        u8 sampleOutput = 0;
        u16 sampleAddr = 0;
        u16 sampleBytesRemaining = 0;
        u16 sampleLoop = 0;
        bool sampleLoopEnabled = false;
        u8 sampleRate = 0;

        // Expansion FM
        u8 carrier = 0;
        u8 modulator = 0;
        bool fmEnabled = false;
        u8 fmIndex = 0;
        u8 fmRatio = 0;
        u32 fmPhase = 0;
        u32 fmModPhase = 0;
        u32 fmModPhaseInc = 0;

        // Modulation (from tracker)
        u8 lfoMode = 0; // 0=off, 1=vibrato, 2=tremolo
        u8 lfoDepth = 0;
        u8 lfoRate = 0;
        u32 lfoCounter = 0;
        u32 lfoPhase2 = 0;

        // Arpeggio
        u8 arpMode = 0; // 0=off, 1=up, 2=down, 3=up-down
        u16 arpNotes[3] = {0, 0, 0};
        u8 arpIndex = 0;
        u8 arpTimer = 0;
        u8 arpSpeed = 1;

        // Volume envelope
        u8 volumeEnvTarget = 0;
        u8 volumeEnvRate = 0;
        u8 volumeEnvCounter = 0;
        u8 volumeEnvCurrent = 0;
        bool volumeEnvActive = false;

        // Pitch slide (portamento)
        u32 slideTarget = 0;
        u32 slideRate = 0;
        u32 slideAccumulator = 0;
        u32 slideDirection = 0;
    };

    // The 9-voice APU with mod tracker.
    class APU
    {
    public:
        static constexpr u8 kVoiceCount = 9;

        APU() { voices_.resize(kVoiceCount); }

        // CPU-visible register interface ($4000-$401F).
        [[nodiscard]] u8 read(u16 address);
        void write(u16 address, u8 value);

        // Called once per CPU cycle. Advances synthesis, envelopes, tracker.
        void clock();
        void reset();

        // Audio output: fetch rendered samples (interleaved stereo float).
        [[nodiscard]] u32 sampleRate() const { return 44100; }
        void takeAudioSamples(std::vector<float> &out)
        {
            out.swap(audioBuffer_);
            audioBuffer_.clear();
        }
        void clearAudio() { audioBuffer_.clear(); }

        // ---- Mod tracker host API ----
        void addPattern(const TrackerPattern &pattern) { tracker_.patterns.push_back(pattern); }
        void clearPatterns() { tracker_.patterns.clear(); }
        void startTracker()
        {
            tracker_.playing = true;
            tracker_.tickCounter = 0;
        }
        void stopTracker() { tracker_.playing = false; }
        void setTrackerPattern(u16 index)
        {
            if (index < tracker_.patterns.size())
                tracker_.patternIndex = index;
        }
        void setTrackerRow(u16 row)
        {
            tracker_.rowIndex = row < tracker_.patterns[tracker_.patternIndex].rows
                                    ? row
                                    : tracker_.patterns[tracker_.patternIndex].rows;
        }
        void setTrackerSpeed(u8 speed) { tracker_.speed = speed < 1 ? 1 : speed; }
        [[nodiscard]] bool isTrackerPlaying() const { return tracker_.playing; }
        [[nodiscard]] u16 trackerRow() const { return tracker_.rowIndex; }
        [[nodiscard]] u16 trackerPattern() const { return tracker_.patternIndex; }

        // Direct voice control (bypasses tracker, for tests or host code).
        void setVoiceEnabled(u8 voice, bool on);
        void setVoiceType(u8 voice, VoiceType type);
        void setVoicePeriod(u8 voice, u16 period);
        void setVoiceVolume(u8 voice, u8 volume);
        void setVoiceDuty(u8 voice, u8 duty);
        void setVoicePan(u8 voice, u8 pan);
        void triggerVoiceMod(u8 voice, TrackerEffect effect, u8 param);
        [[nodiscard]] u8 voiceCount() const { return kVoiceCount; }

    private:
        void clockVoice(u8 voiceIndex);
        void clockTracker();
        void processTrackerCell(u8 voiceIndex, const TrackerCell &cell);
        void mixVoice(u8 voiceIndex, float &left, float &right);
        [[nodiscard]] u32 periodToPhaseInc(u16 period) const;
        [[nodiscard]] u32 noteToPeriod(u16 note) const;
        [[nodiscard]] float sampleVoice(u8 voiceIndex);
        void resetVoice(u8 voiceIndex);

        std::vector<Voice> voices_;
        std::array<u8, 0x18> registers_{};
        u8 status_ = 0;
        u8 frameCounter_ = 0;
        u8 frameCounterMode_ = 0;
        u8 frameIrqInhibit_ = 0;
        u8 frameStep_ = 0;
        u32 frameDivider_ = 0;

        // Audio synthesis
        u32 audioSampleCounter_ = 0;
        u32 audioSampleRate_ = 44100;
        u32 audioSampleAccumulator_ = 0;
        float audioSampleLeft_ = 0.0f;
        float audioSampleRight_ = 0.0f;
        std::vector<float> audioBuffer_;

        // Mod tracker
        Tracker tracker_;
    };

}
