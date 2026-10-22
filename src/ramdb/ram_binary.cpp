#include "nex8/ramdb/ram_binary.hpp"

#include <array>
#include <fstream>
#include <limits>
#include <utility>

namespace nex8::ramdb {
namespace {

constexpr std::array<std::uint8_t, 4> Magic{'N', '8', 'D', 'B'};
constexpr std::uint8_t Version = 1;

class Writer {
public:
    void u8(std::uint8_t v) { bytes.push_back(v); }
    void u16(std::uint16_t v) { u8(static_cast<std::uint8_t>(v & 0xFF)); u8(static_cast<std::uint8_t>((v >> 8) & 0xFF)); }
    void u32(std::uint32_t v) { u16(static_cast<std::uint16_t>(v)); u16(static_cast<std::uint16_t>(v >> 16)); }
    void text(const std::string& s) {
        u16(static_cast<std::uint16_t>(s.size()));
        bytes.insert(bytes.end(), s.begin(), s.end());
    }
    std::vector<std::uint8_t> bytes;
};

class Reader {
public:
    explicit Reader(const std::vector<std::uint8_t>& data) : data_(data) {}
    bool u8(std::uint8_t& v) { if (!need(1)) return false; v = data_[pos_++]; return true; }
    bool u16(std::uint16_t& v) {
        std::uint8_t a{}, b{}; if (!u8(a) || !u8(b)) return false;
        v = static_cast<std::uint16_t>(a | (static_cast<std::uint16_t>(b) << 8)); return true;
    }
    bool u32(std::uint32_t& v) {
        std::uint16_t a{}, b{}; if (!u16(a) || !u16(b)) return false;
        v = static_cast<std::uint32_t>(a | (static_cast<std::uint32_t>(b) << 16)); return true;
    }
    bool text(std::string& s) {
        std::uint16_t n{}; if (!u16(n) || !need(n)) return false;
        s.assign(reinterpret_cast<const char*>(data_.data() + pos_), n); pos_ += n; return true;
    }
    bool need(std::size_t n) const { return n <= data_.size() - pos_; }
    bool atEnd() const { return pos_ == data_.size(); }
private:
    const std::vector<std::uint8_t>& data_;
    std::size_t pos_{0};
};

void fail(std::string* error, const char* message) { if (error) *error = message; }

bool writeValue(Writer& w, const Value& value, ValueType type) {
    switch (type) {
    case ValueType::UInt8:  w.u8(std::get<std::uint8_t>(value)); return true;
    case ValueType::UInt16: w.u16(std::get<std::uint16_t>(value)); return true;
    case ValueType::UInt32: w.u32(std::get<std::uint32_t>(value)); return true;
    case ValueType::Int8:   w.u8(static_cast<std::uint8_t>(std::get<std::int8_t>(value))); return true;
    case ValueType::Int16:  w.u16(static_cast<std::uint16_t>(std::get<std::int16_t>(value))); return true;
    case ValueType::Int32:  w.u32(static_cast<std::uint32_t>(std::get<std::int32_t>(value))); return true;
    case ValueType::Bool:   w.u8(std::get<bool>(value) ? 1 : 0); return true;
    case ValueType::Text:   w.text(std::get<std::string>(value)); return true;
    }
    return false;
}

bool readValue(Reader& r, ValueType type, Value& value) {
    std::uint8_t u8{}; std::uint16_t u16{}; std::uint32_t u32{}; std::string text;
    switch (type) {
    case ValueType::UInt8:  if (!r.u8(u8)) return false; value = u8; return true;
    case ValueType::UInt16: if (!r.u16(u16)) return false; value = u16; return true;
    case ValueType::UInt32: if (!r.u32(u32)) return false; value = u32; return true;
    case ValueType::Int8:   if (!r.u8(u8)) return false; value = static_cast<std::int8_t>(u8); return true;
    case ValueType::Int16:  if (!r.u16(u16)) return false; value = static_cast<std::int16_t>(u16); return true;
    case ValueType::Int32:  if (!r.u32(u32)) return false; value = static_cast<std::int32_t>(u32); return true;
    case ValueType::Bool:   if (!r.u8(u8) || u8 > 1) return false; value = (u8 != 0); return true;
    case ValueType::Text:   if (!r.text(text)) return false; value = std::move(text); return true;
    }
    return false;
}

} // namespace

BinaryImage serialize(const Database& database, std::string* error) {
    BinaryImage image;
    Writer w;
    for (const auto c : Magic) w.u8(c);
    w.u8(Version);
    const auto names = database.tableNames();
    if (names.size() > std::numeric_limits<std::uint16_t>::max()) { fail(error, "too many tables"); return {}; }
    w.u16(static_cast<std::uint16_t>(names.size()));

    for (const auto& name : names) {
        const Table* table = database.table(name);
        if (!table) { fail(error, "table disappeared during serialization"); return {}; }
        const auto& schema = table->schema();
        if (!schema.valid(error) || schema.columns.size() > std::numeric_limits<std::uint16_t>::max()) return {};
        w.text(schema.name);
        w.u16(schema.rowSize);
        w.u16(schema.maxRows);
        w.u16(static_cast<std::uint16_t>(schema.columns.size()));
        for (const auto& column : schema.columns) {
            w.text(column.name);
            w.u8(static_cast<std::uint8_t>(column.type));
            w.u16(column.offset);
            w.u16(column.size);
        }
        const auto rows = table->selectAll();
        if (rows.size() > std::numeric_limits<std::uint16_t>::max()) { fail(error, "too many rows"); return {}; }
        w.u16(static_cast<std::uint16_t>(rows.size()));
        for (const auto& row : rows) {
            w.u16(row.id);
            if (row.values.size() != schema.columns.size()) { fail(error, "row/schema mismatch"); return {}; }
            for (std::size_t i = 0; i < row.values.size(); ++i) {
                if (!writeValue(w, row.values[i], schema.columns[i].type)) { fail(error, "invalid value type"); return {}; }
            }
        }
    }
    image.bytes = std::move(w.bytes);
    return image;
}

bool deserialize(const std::vector<std::uint8_t>& bytes, Database& database, std::string* error) {
    Reader r(bytes);
    for (const auto expected : Magic) { std::uint8_t actual{}; if (!r.u8(actual) || actual != expected) { fail(error, "invalid N8DB magic"); return false; } }
    std::uint8_t version{}; if (!r.u8(version) || version != Version) { fail(error, "unsupported N8DB version"); return false; }
    std::uint16_t tableCount{}; if (!r.u16(tableCount)) { fail(error, "truncated N8DB header"); return false; }

    Database decoded;
    for (std::uint16_t t = 0; t < tableCount; ++t) {
        TableSchema schema;
        std::uint16_t columnCount{};
        if (!r.text(schema.name) || !r.u16(schema.rowSize) || !r.u16(schema.maxRows) || !r.u16(columnCount)) { fail(error, "truncated N8DB table schema"); return false; }
        schema.columns.reserve(columnCount);
        for (std::uint16_t c = 0; c < columnCount; ++c) {
            Column column;
            std::uint8_t type{};
            if (!r.text(column.name) || !r.u8(type) || !r.u16(column.offset) || !r.u16(column.size) || type > static_cast<std::uint8_t>(ValueType::Text)) { fail(error, "invalid N8DB column"); return false; }
            column.type = static_cast<ValueType>(type);
            schema.columns.push_back(std::move(column));
        }
        if (!schema.valid(error) || !decoded.createTable(schema, error)) return false;
        Table* table = decoded.table(schema.name);
        std::uint16_t rowCount{};
        if (!table || !r.u16(rowCount)) { fail(error, "truncated N8DB rows"); return false; }
        if (rowCount > schema.maxRows) { fail(error, "N8DB row count exceeds table capacity"); return false; }
        for (std::uint16_t i = 0; i < rowCount; ++i) {
            std::uint16_t id{}; if (!r.u16(id)) { fail(error, "truncated N8DB row id"); return false; }
            Row row; row.reserve(schema.columns.size());
            for (const auto& column : schema.columns) {
                Value value; if (!readValue(r, column.type, value)) { fail(error, "truncated or invalid N8DB value"); return false; }
                row.push_back(std::move(value));
            }
            if (!table->restore(static_cast<RowId>(id), row, error)) return false;
        }
    }
    if (!r.atEnd()) { fail(error, "trailing bytes in N8DB image"); return false; }
    database = std::move(decoded);
    return true;
}

bool saveBinary(const Database& database, const std::string& path, std::string* error) {
    const BinaryImage image = serialize(database, error);
    if (image.bytes.empty()) return false;
    std::ofstream out(path, std::ios::binary);
    if (!out) { fail(error, "unable to open N8DB output file"); return false; }
    out.write(reinterpret_cast<const char*>(image.bytes.data()), static_cast<std::streamsize>(image.bytes.size()));
    if (!out) { fail(error, "unable to write N8DB output file"); return false; }
    return true;
}

bool loadBinary(const std::string& path, Database& database, std::string* error) {
    std::ifstream in(path, std::ios::binary);
    if (!in) { fail(error, "unable to open N8DB input file"); return false; }
    in.seekg(0, std::ios::end); const auto end = in.tellg(); in.seekg(0, std::ios::beg);
    if (end < 0) { fail(error, "unable to determine N8DB file size"); return false; }
    const auto size = static_cast<std::size_t>(end);
    std::vector<std::uint8_t> bytes(size);
    if (size != 0) in.read(reinterpret_cast<char*>(bytes.data()), static_cast<std::streamsize>(size));
    if (!in && size != 0) { fail(error, "unable to read N8DB input file"); return false; }
    return deserialize(bytes, database, error);
}

} // namespace nex8::ramdb
