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

std::vector<std::string> filter_empty_strings(const std::vector<std::string>& tab);

std::string strip(const std::string& str);

bool ends_with(std::string const& value, std::string const& ending);

std::string demangle_function_name(const std::string& mangled_name);

#endif //CERBERUS_CONVERT_H
