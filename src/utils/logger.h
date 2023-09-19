#ifndef CERBERUS_LOGGER_H
#define CERBERUS_LOGGER_H

#include <string>
#include <map>
#include <vector>
#include <cstdint>

extern std::map<std::string, uint8_t> LOG_COLORS;
extern std::map<std::string, std::pair<uint8_t, uint8_t>> LOG_LEVELS;
extern std::map<char, uint8_t> LOG_STYLES;

class FCout {
private:
    std::vector<std::pair<uint8_t,uint8_t>> args_stack;
public:
    FCout operator<<(std::string s);
    FCout operator<<(std::ostream&(*pManip)(std::ostream&));
    void format(std::string& s);
};

extern FCout fcout;

#endif //CERBERUS_LOGGER_H
