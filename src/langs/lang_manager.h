#ifndef CERBERUS_LANG_MANAGER_H
#define CERBERUS_LANG_MANAGER_H

#include <string>
#include <map>
#include <vector>
#include <types/value_ordered_map.h>

enum LANG {
    UNKNOWN_LANG,
    RUST,
    GO
};

extern std::map<LANG, std::string> name_from_lang;

extern std::vector<std::pair<std::string, LANG>> LANG_PATTERNS;

class LangIdentifier {
private:
    std::string input_path;
public:
    LangIdentifier(std::string input_path);
    value_ordered_map<LANG, size_t> identify();
};

#endif //CERBERUS_LANG_MANAGER_H
