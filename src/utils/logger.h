#ifndef CERBERUS_LOGGER_H
#define CERBERUS_LOGGER_H

#include <string>
#include <map>

extern std::map<std::string, uint8_t> LOG_COLORS;

class FCout {
private:
    void format(std::string& s);
public:
    FCout operator<<(std::string s);
    FCout operator<<(std::ostream&(*pManip)(std::ostream&));
};

extern FCout fcout;

#endif //CERBERUS_LOGGER_H
