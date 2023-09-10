#include <binaries/handlers/elf_handler.h>
#include <vector>
#include <iostream>

using namespace std;

void ElfHandler::strip_analysis() {
    this->stripped = true;
    vector<SECTION*> sections = this->lief_extractor->extract_sections();
    const vector<string> debug_sections = {".symtab", ".strtab"};
    for(SECTION* section : sections) {
        if(find(debug_sections.begin(), debug_sections.end(), section->name) != debug_sections.end()) {
            this->stripped = false;
            return;
        }
    }
}

void ElfHandler::libs_analysis() {

}

size_t ElfHandler::functions_analysis() {

}

size_t ElfHandler::functions_matching() {

}