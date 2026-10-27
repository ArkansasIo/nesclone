#include "nex8/ramdb/ram_sql.hpp"

#include <algorithm>
#include <cctype>
#include <charconv>
#include <sstream>
#include <string_view>
#include <vector>

namespace nex8::ramdb {
namespace {

std::string upper(std::string value) {
    std::transform(value.begin(), value.end(), value.begin(), [](unsigned char c) { return static_cast<char>(std::toupper(c)); });
    return value;
}

std::vector<std::string> words(const std::string& text) {
    std::istringstream in(text);
    std::vector<std::string> out;
    std::string word;
    while (in >> word) out.push_back(word);
    return out;
}

bool parseUnsigned(const std::string& token, std::uint32_t& value) {
    const auto* begin = token.data();
    const auto* end = token.data() + token.size();
    const auto result = std::from_chars(begin, end, value);
    return result.ec == std::errc{} && result.ptr == end;
}

ValueType parseType(const std::string& token, bool& ok) {
    const auto type = upper(token);
    ok = true;
    if (type == "UINT8") return ValueType::UInt8;
    if (type == "UINT16") return ValueType::UInt16;
    if (type == "UINT32") return ValueType::UInt32;
    if (type == "INT8") return ValueType::Int8;
    if (type == "INT16") return ValueType::Int16;
    if (type == "INT32") return ValueType::Int32;
    if (type == "BOOL") return ValueType::Bool;
    if (type == "TEXT") return ValueType::Text;
    ok = false;
    return ValueType::UInt8;
}

std::string stripPunctuation(std::string value) {
    while (!value.empty() && (value.front() == '(' || value.front() == ',')) value.erase(value.begin());
    while (!value.empty() && (value.back() == ')' || value.back() == ',' || value.back() == ';')) value.pop_back();
    return value;
}

bool parseValue(const std::string& token, ValueType type, Value& out) {
    const auto clean = stripPunctuation(token);
    if (type == ValueType::Text) {
        if (clean.size() >= 2 && ((clean.front() == '\'' && clean.back() == '\'') || (clean.front() == '"' && clean.back() == '"')))
            out = clean.substr(1, clean.size() - 2);
        else out = clean;
        return true;
    }
    if (type == ValueType::Bool) {
        const auto value = upper(clean);
        if (value == "TRUE" || value == "1") { out = true; return true; }
        if (value == "FALSE" || value == "0") { out = false; return true; }
        return false;
    }
    std::uint32_t number = 0;
    if (!parseUnsigned(clean, number)) return false;
    switch (type) {
    case ValueType::UInt8: out = static_cast<std::uint8_t>(number); return number <= 255;
    case ValueType::UInt16: out = static_cast<std::uint16_t>(number); return number <= 65535;
    case ValueType::UInt32: out = number; return true;
    default: return false;
    }
}

SqlResult error(std::string message) { return {false, std::move(message), 0}; }
SqlResult success(std::string message, std::size_t affected = 0) { return {true, std::move(message), affected}; }

} // namespace

SqlResult execute(Database& database, const std::string& statement) {
    const auto tokens = words(statement);
    if (tokens.empty()) return error("empty statement");
    const auto command = upper(tokens[0]);

    if (command == "CREATE" && tokens.size() >= 4 && upper(tokens[1]) == "TABLE") {
        TableSchema schema;
        schema.name = stripPunctuation(tokens[2]);
        std::size_t i = 3;
        while (i < tokens.size()) {
            if (upper(tokens[i]) == "ENDTABLE") break;
            if (upper(tokens[i]) != "COLUMN" || i + 2 >= tokens.size()) return error("expected COLUMN name TYPE");
            const std::string name = stripPunctuation(tokens[i + 1]);
            bool typeOk = false;
            const auto type = parseType(stripPunctuation(tokens[i + 2]), typeOk);
            if (!typeOk) return error("unknown column type");
            Column column{name, type, schema.rowSize, valueSize(type)};
            if (type == ValueType::Text) {
                column.size = 32;
                if (i + 3 < tokens.size() && upper(tokens[i + 3]).starts_with("SIZE=")) {
                    std::uint32_t size = 0;
                    if (!parseUnsigned(tokens[i + 3].substr(5), size) || size == 0 || size > 255) return error("invalid TEXT size");
                    column.size = static_cast<std::uint16_t>(size);
                    ++i;
                }
            }
            schema.columns.push_back(column);
            schema.rowSize = static_cast<std::uint16_t>(schema.rowSize + column.size);
            i += 3;
            continue;
        }
        schema.maxRows = 256;
        std::string validation;
        return database.createTable(std::move(schema), &validation) ? success("table created") : error(validation);
    }

    if (command == "DROP" && tokens.size() == 3 && upper(tokens[1]) == "TABLE")
        return database.dropTable(tokens[2]) ? success("table dropped") : error("table not found");

    if (command == "INSERT" && tokens.size() >= 5 && upper(tokens[1]) == "INTO") {
        auto* table = database.table(tokens[2]);
        if (!table) return error("table not found");
        std::unordered_map<std::string, Value> fields;
        std::size_t i = 3;
        if (upper(tokens[i]) == "VALUES") ++i;
        for (const auto& column : table->schema().columns) {
            if (i >= tokens.size()) return error("not enough values");
            Value value;
            if (!parseValue(tokens[i++], column.type, value)) return error("invalid value for " + column.name);
            fields.emplace(column.name, std::move(value));
        }
        std::string message;
        RowId id = 0;
        return table->insert(fields, &id, &message) ? success("row inserted id=" + std::to_string(id), 1) : error(message);
    }

    if (command == "SELECT" && tokens.size() >= 4) {
        auto* table = database.table(tokens.back());
        if (!table) return error("table not found");
        return success("rows=" + std::to_string(table->size()), table->size());
    }

    return error("unsupported RAMSQL statement");
}

} // namespace nex8::ramdb
