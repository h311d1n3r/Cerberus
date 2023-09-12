#ifndef CERBERUS_BIN_TYPES_H
#define CERBERUS_BIN_TYPES_H

#include <cstdint>
#include <string>

struct FUNCTION {
    uint64_t start;
    uint64_t end;
    std::string name;
    std::string hash;
    uint8_t score = 0;
};

struct SECTION {
    uint64_t start;
    uint64_t end;
    std::string name;
};

struct LIBRARY {
    std::string name;
    std::string version;
};

enum BIN_TYPE {
    UNKNOWN_TYPE,
    ELF,
    PE,
};

enum BIN_ARCH {
    UNKNOWN_ARCH,
    X86_64,
    X86
};

#endif //CERBERUS_BIN_TYPES_H
