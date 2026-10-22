#include "nex8/ramdb/ram_binary.hpp"

#include <iostream>
#include <string>

namespace {
void usage() {
    std::cout << "Usage:\n"
              << "  n8db inspect <file.n8db>\n"
              << "  n8db copy <input.n8db> <output.n8db>\n";
}
}

int main(int argc, char** argv) {
    using namespace nex8::ramdb;
    if (argc < 3) { usage(); return 2; }
    const std::string command = argv[1];
    const std::string input = argv[2];
    Database database;
    std::string error;
    if (!loadBinary(input, database, &error)) {
        std::cerr << "N8DB load failed: " << error << '\n';
        return 1;
    }

    if (command == "inspect") {
        std::cout << "N8DB v1\n";
        for (const auto& name : database.tableNames()) {
            const auto* table = database.table(name);
            std::cout << "TABLE " << name << " rows=" << table->size()
                      << " max=" << table->schema().maxRows << '\n';
            for (const auto& column : table->schema().columns) {
                std::cout << "  COLUMN " << column.name
                          << " type=" << static_cast<unsigned>(column.type)
                          << " offset=" << column.offset
                          << " size=" << column.size << '\n';
            }
        }
        return 0;
    }

    if (command == "copy" && argc == 4) {
        if (!saveBinary(database, argv[3], &error)) {
            std::cerr << "N8DB save failed: " << error << '\n';
            return 1;
        }
        return 0;
    }

    usage();
    return 2;
}
