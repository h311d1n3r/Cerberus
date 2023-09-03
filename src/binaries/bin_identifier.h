#ifndef CERBERUS_BIN_IDENTIFIER_H
#define CERBERUS_BIN_IDENTIFIER_H

#include <map>
#include <string>

enum BIN_TYPE {
    UNKNOWN_TYPE,
    ELF,
    PE,
};

extern std::map<BIN_TYPE, std::string> bin_type_names;

extern std::map<std::string, BIN_TYPE> bin_type_from_magic;

BIN_TYPE identify_binary(std::string input_path);

#endif //CERBERUS_BIN_IDENTIFIER_H
