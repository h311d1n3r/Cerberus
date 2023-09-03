#ifndef CERBERUS_BIN_TYPES_H
#define CERBERUS_BIN_TYPES_H

#include <cstdint>

struct FUNCTION {
    uint64_t start;
    uint64_t end;
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

#endif //CERBERUS_BIN_TYPES_H
