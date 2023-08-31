#ifndef CERBERUS_LANG_MANAGER_H
#define CERBERUS_LANG_MANAGER_H

#include <string>
#include <map>
#include <vector>

enum LANG {
    Rust,
    Go
};

extern std::map<LANG, std::string> name_from_lang;

extern std::vector<std::pair<std::string, LANG>> LANG_PATTERNS;

class LangIdentifier {
};

#endif //CERBERUS_LANG_MANAGER_H
