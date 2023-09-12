#include <binaries/handlers/elf_handler.h>
#include <vector>
#include <iostream>

using namespace std;

void ElfHandler::strip_analysis() {
    this->stripped = true;
    vector<unique_ptr<SECTION>> sections = this->lief_extractor->extract_sections();
    const vector<string> debug_sections = {".symtab", ".strtab"};
    for(unique_ptr<SECTION>& section : sections) {
        if(find(debug_sections.begin(), debug_sections.end(), section->name) != debug_sections.end()) {
            this->stripped = false;
            return;
        }
    }
}

size_t ElfHandler::functions_analysis() {
    vector<unique_ptr<FUNCTION>> funcs = this->lief_extractor->extract_functions();
    if(!funcs.size()) funcs = this->radare_extractor->extract_functions();
    algorithm->process_binary(&funcs);
    return funcs.size();
}

void ElfHandler::functions_matching(string lib_path) {
    LiefExtractor lib_lief_extractor(lib_path);
    RadareExtractor lib_radare_extractor(lib_path);
    vector<unique_ptr<FUNCTION>> funcs = lib_lief_extractor.extract_functions();
    if(!funcs.size()) funcs = lib_radare_extractor.extract_functions();
    algorithm->process_lib(lib_path, &funcs);
}