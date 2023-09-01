#include <langs/lang_manager.h>
#include <utils/logger.h>
#include <fstream>
#include <cstring>
#include <algorithm>

using namespace std;

map<LANG, string> name_from_lang = {
    {LANG::Rust, "Rust"},
    {LANG::Go, "Go"}
};

vector<pair<string, LANG>> LANG_PATTERNS = {
    std::pair("/rustc-", LANG::Rust),
    std::pair("/.cargo/", LANG::Rust),
    std::pair("/go-", LANG::Go),
    std::pair("runtime.go", LANG::Go)
};

LangIdentifier::LangIdentifier(string elf_path) {
    this->elf_path = elf_path;
}

value_ordered_map<LANG, size_t> LangIdentifier::identify() {
    value_ordered_map<LANG, size_t> matches;
    for(uint32_t i = 0; i < sizeof(LANG); i++) matches[(LANG)i] = 0;
    matches[Rust]++;
    matches.sort();
    for (size_t i = 0 ; i < matches.size(); i++) {
        pair<LANG, size_t> match = matches.at(i);
        fcout << "Key: " << name_from_lang[match.first] << ", Value: " << to_string(match.second) << std::endl;
    }
    ifstream elf_file(this->elf_path);
    if (!elf_file.is_open()) {
        fcout << "$(critical)File $(critical:u)" << elf_path << "$ can't be opened !" << endl;
        exit(1);
    }
    elf_file.seekg(0, ios::end);
    size_t elf_sz = elf_file.tellg();
    elf_file.seekg(0);
    char elf_data[elf_sz];
    elf_file.read(elf_data, elf_sz);
    for(pair<string, LANG> lang_pattern : LANG_PATTERNS) {
        char* current_pos = elf_data;
        while((current_pos = strstr(current_pos, lang_pattern.first.c_str())) != nullptr) {
            matches[lang_pattern.second]++;
            current_pos += lang_pattern.first.length();
        }
    }
    return matches;
}