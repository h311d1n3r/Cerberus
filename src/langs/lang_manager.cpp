#include <langs/lang_manager.h>
#include <utils/logger.h>
#include <fstream>
#include <cstring>
#include <algorithm>

using namespace std;

map<LANG, string> name_from_lang = {
    {LANG::UNKNOWN_LANG, "Unknown"},
    {LANG::RUST, "Rust"},
    {LANG::GO, "Go"}
};

vector<pair<string, LANG>> LANG_PATTERNS = {
    std::pair("/rustc-", LANG::RUST),
    std::pair("/.cargo/", LANG::RUST),
    std::pair("/go-", LANG::GO),
    std::pair("runtime.go", LANG::GO)
};

LangIdentifier::LangIdentifier(string input_path) {
    this->input_path = input_path;
}

value_ordered_map<LANG, size_t> LangIdentifier::identify() {
    value_ordered_map<LANG, size_t> matches;
    for(uint32_t i = 1; i < name_from_lang.size(); i++) matches[(LANG)i] = 0;
    ifstream input_file(this->input_path);
    if (!input_file.is_open()) {
        fcout << "$(critical)File $(critical:u)" << input_path << "$ can't be opened !" << endl;
        exit(1);
    }
    input_file.seekg(0, ios::end);
    size_t input_sz = input_file.tellg();
    input_file.seekg(0);
    char input_data[input_sz];
    input_file.read(input_data, input_sz);
    for(pair<string, LANG> lang_pattern : LANG_PATTERNS) {
        char* current_pos = input_data;
        char* occurrence;
        while(current_pos-input_data < input_sz) {
            if ((occurrence = strstr(current_pos, lang_pattern.first.c_str())) != nullptr) {
                current_pos = occurrence + lang_pattern.first.length();
                matches[lang_pattern.second]++;
            } else current_pos++;
        }
    }
    input_file.close();
    matches.invert_sort();
    return matches;
}