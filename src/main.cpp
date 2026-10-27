#include "nes/apu/apu.hpp"
#include "nes/nes/nes.hpp"
#include <cctype>
#include <cstdint>
#include <cstdlib>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <vector>

namespace
{

    // Build a short demo song: 4 patterns of tracker rows, then play it.
    void loadDemoSong(nes::APU &apu)
    {
        using nes::TrackerCell;
        using nes::TrackerPattern;
        // 32 rows, 9 voices. Voice 0: pulse melody. Voice 1: pulse bass. Voice 2: triangle.
        // Voice 3: noise drums. Voice 5: FM lead with vibrato.
        TrackerPattern pat(32, 9);
        // Melody: A4 scale-ish loop every 2 rows.
        static const std::uint16_t melody[8] = {69, 71, 72, 74, 76, 74, 72, 71};
        static const std::uint16_t bass[8] = {45, 45, 52, 52, 48, 48, 45, 45};
        for (int row = 0; row < 32; ++row)
        {
            // Melody voice 0
            TrackerCell &mel = pat.cell(row, 0);
            mel.note = melody[(row / 2) % 8];
            mel.volume = 12;
            mel.effect = row % 4 == 0 ? 0x02 : 0xFF; // vibrato on some rows
            mel.effectParam = row % 4 == 0 ? 0x35 : 0;
            // Bass voice 1
            TrackerCell &b = pat.cell(row, 1);
            b.note = bass[(row / 4) % 8];
            b.volume = 10;
            // Triangle voice 2: sustained low note
            if (row % 8 == 0)
            {
                TrackerCell &t = pat.cell(row, 2);
                t.note = 36;
                t.volume = 10;
            }
            // Noise drums voice 3: kick on beat 0, snare on beat 2
            if (row % 4 == 0)
            {
                TrackerCell &d = pat.cell(row, 3);
                d.note = 0xFFFF; // no pitch change
                d.volume = row % 8 == 0 ? 12 : 8;
                d.effect = 0x05; // volume envelope
                d.effectParam = row % 8 == 0 ? 0x20 : 0x10;
            }
            // FM lead voice 5: echo of melody with vibrato
            TrackerCell &fm = pat.cell(row, 5);
            fm.note = melody[(row / 2) % 8] + 12;
            fm.volume = 9;
            fm.effect = row % 8 == 0 ? 0x02 : 0xFF;
            fm.effectParam = row % 8 == 0 ? 0x28 : 0;
        }
        apu.addPattern(pat);

        // Voice setup: types and panning.
        apu.setVoiceType(0, nes::VoiceType::Pulse);
        apu.setVoiceDuty(0, 2);
        apu.setVoicePan(0, 128);
        apu.setVoiceType(1, nes::VoiceType::Pulse);
        apu.setVoiceDuty(1, 1);
        apu.setVoicePan(1, 128);
        apu.setVoiceType(2, nes::VoiceType::Triangle);
        apu.setVoicePan(2, 128);
        apu.setVoiceType(3, nes::VoiceType::Noise);
        apu.setVoicePan(3, 128);
        apu.setVoiceType(5, nes::VoiceType::Fm);
        apu.setVoicePan(5, 200);
        apu.setVoiceEnabled(0, true);
        apu.setVoiceEnabled(1, true);
        apu.setVoiceEnabled(2, true);
        apu.setVoiceEnabled(3, true);
        apu.setVoiceEnabled(5, true);
        apu.setTrackerSpeed(6);
        apu.startTracker();
    }

    void writeWav(const std::filesystem::path &path, const std::vector<float> &interleaved,
                  std::uint32_t sampleRate)
    {
        std::ofstream out(path, std::ios::binary);
        if (!out)
            return;
        const std::uint32_t numSamples = static_cast<std::uint32_t>(interleaved.size() / 2);
        const std::uint32_t dataBytes = numSamples * 2 * 2; // stereo, 16-bit
        const std::uint8_t riff[4] = {'R', 'I', 'F', 'F'};
        const std::uint32_t riffSize = 36 + dataBytes;
        const std::uint8_t wave[4] = {'W', 'A', 'V', 'E'};
        const std::uint8_t fmt[4] = {'f', 'm', 't', ' '};
        const std::uint32_t fmtSize = 16;
        const std::uint16_t audioFormat = 1;
        const std::uint16_t channels = 2;
        const std::uint16_t bitsPerSample = 16;
        const std::uint16_t blockAlign = static_cast<std::uint16_t>(channels * bitsPerSample / 8);
        const std::uint32_t byteRate = sampleRate * blockAlign;
        const std::uint8_t data[4] = {'d', 'a', 't', 'a'};
        out.write(reinterpret_cast<const char *>(riff), 4);
        out.write(reinterpret_cast<const char *>(&riffSize), 4);
        out.write(reinterpret_cast<const char *>(wave), 4);
        out.write(reinterpret_cast<const char *>(fmt), 4);
        out.write(reinterpret_cast<const char *>(&fmtSize), 4);
        out.write(reinterpret_cast<const char *>(&audioFormat), 2);
        out.write(reinterpret_cast<const char *>(&channels), 2);
        out.write(reinterpret_cast<const char *>(&sampleRate), 4);
        out.write(reinterpret_cast<const char *>(&byteRate), 4);
        out.write(reinterpret_cast<const char *>(&blockAlign), 2);
        out.write(reinterpret_cast<const char *>(&bitsPerSample), 2);
        out.write(reinterpret_cast<const char *>(data), 4);
        out.write(reinterpret_cast<const char *>(&dataBytes), 4);
        for (float f : interleaved)
        {
            float clamped = f;
            if (clamped > 1.0f)
                clamped = 1.0f;
            if (clamped < -1.0f)
                clamped = -1.0f;
            const auto s = static_cast<std::int16_t>(clamped * 32767.0f);
            out.write(reinterpret_cast<const char *>(&s), 2);
        }
    }

} // namespace

int main(int argc, char **argv)
{
    if (argc < 2)
    {
        std::cerr << "Usage: nesclone <rom.nes> [--audio-out <file.wav> [--frames <n>]]\n";
        return 2;
    }
    std::filesystem::path audioOut;
    std::uint64_t runFrames = 0;
    for (int i = 2; i + 1 < argc; i += 2)
    {
        if (std::string(argv[i]) == "--audio-out")
            audioOut = argv[i + 1];
        else if (std::string(argv[i]) == "--frames")
            runFrames = std::strtoull(argv[i + 1], nullptr, 10);
    }
    nes::NES system;
    std::string error;
    if (!system.loadCartridge(argv[1], error))
    {
        std::cerr << "ROM load failed: " << error << '\n';
        return 1;
    }
    const auto *image = system.cartridge();
    std::cout << "NESClone initial milestone\n"
              << "Mapper: " << image->mapperId << '\n'
              << "PRG-ROM: " << image->prgRom.size() << " bytes\n"
              << "CHR-ROM: " << image->chrRom.size() << " bytes\n"
              << "CHR-RAM: " << image->chrRam.size() << " bytes\n"
              << "Mirroring: " << (image->fourScreen ? "four-screen" : (image->verticalMirroring ? "vertical" : "horizontal")) << '\n'
              << "CPU reset vector: $" << std::hex << system.cpu().registers().pc << '\n';

    // Demo song: play the 9-voice mod tracker regardless of ROM audio.
    if (!audioOut.empty())
    {
        loadDemoSong(system.apu());
        std::vector<float> samples;
        const std::uint64_t totalFrames = runFrames > 0 ? runFrames : 600; // 600 CPU frames ~= 10s
        for (std::uint64_t f = 0; f < totalFrames; ++f)
        {
            for (int c = 0; c < 29780; ++c)
                system.clock(); // ~1 frame of CPU cycles
            std::vector<float> chunk;
            system.apu().takeAudioSamples(chunk);
            samples.insert(samples.end(), chunk.begin(), chunk.end());
        }
        std::cout << "Rendered " << (samples.size() / 2) << " samples\n";
        writeWav(audioOut, samples, system.apu().sampleRate());
        std::cout << "Audio written to " << audioOut.string() << '\n';
    }
    return 0;
}
