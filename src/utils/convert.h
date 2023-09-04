#ifndef CERBERUS_CONVERT_H
#define CERBERUS_CONVERT_H

#include <string>
#include <vector>

template<typename IntType> bool string_to_int(std::string s, IntType& val) {
    try {
        if (s.size() >= 2 && s.substr(0, 2) == "0x") val = static_cast<IntType>(stoll(s.substr(2), nullptr, 16));
        else val = static_cast<IntType>(stoll(s, nullptr, 10));
        return true;
    } catch (const std::exception& e) {
        return false;
    }
}

std::vector<std::string> split_string(const std::string& input, char delimiter);

#endif //CERBERUS_CONVERT_H
