#pragma once

#include <cstdint>

namespace nex8::ramdb {

// Native NEX8/CELL8 dataset window. The addresses are a cartridge-RAM convention,
// not a claim that the legacy iNES/6502 core exposes this exact hardware.
inline constexpr std::uint16_t DatasetRamBegin = 0x6000;
inline constexpr std::uint16_t DatasetRamEnd   = 0x7FFF;
inline constexpr std::uint16_t DatasetRamSize  = DatasetRamEnd - DatasetRamBegin + 1;

inline constexpr std::uint16_t TableHeaderBegin = 0x6000;
inline constexpr std::uint16_t TableHeaderSize  = 0x0100;
inline constexpr std::uint16_t ColumnMetaBegin  = 0x6100;
inline constexpr std::uint16_t ColumnMetaSize   = 0x0100;
inline constexpr std::uint16_t RowDataBegin     = 0x6200;
inline constexpr std::uint16_t RowDataEnd       = DatasetRamEnd;

struct TableHeader {
    std::uint8_t magic[4]{'N', '8', 'D', 'B'};
    std::uint8_t version{1};
    std::uint8_t flags{0};
    std::uint16_t rowSize{0};
    std::uint16_t maxRows{0};
    std::uint16_t rowCount{0};
    std::uint16_t firstRowOffset{RowDataBegin - DatasetRamBegin};
};

} // namespace nex8::ramdb
