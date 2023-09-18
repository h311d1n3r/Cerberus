#include <binaries/handlers/elf_handler.h>
#include <vector>
#include <binaries/extractors/libelf_extractor.h>

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
    vector<unique_ptr<FUNCTION>> funcs = this->lief_extractor->extract_functions(this->arch, this->image_base);
    if(!funcs.size()) funcs = this->radare_extractor->extract_functions(this->arch, this->image_base);
    if(!funcs.size()) funcs = this->libelf_extractor->extract_functions(this->arch, this->image_base);
    algorithm->process_binary(&funcs);
    this->functions = move(funcs);
    return this->functions.size();
}

void ElfHandler::functions_matching(string lib_path) {
    LiefExtractor lib_lief_extractor(lib_path, type);
    LibelfExtractor lib_libelf_extractor(lib_path);
    RadareExtractor lib_radare_extractor(lib_path);
    size_t lib_image_base = lib_lief_extractor.extract_image_base();
    vector<unique_ptr<FUNCTION>> funcs = lib_lief_extractor.extract_functions(this->arch, lib_image_base);
    if(!funcs.size()) funcs = lib_libelf_extractor.extract_functions(this->arch, lib_image_base);
    if(!funcs.size()) funcs = lib_radare_extractor.extract_functions(this->arch, lib_image_base);
    algorithm->process_lib(lib_path, &funcs);
}

void ElfHandler::post_matching() {
    algorithm->post_process(&functions);
}

bool ElfHandler::write_output(string output_path) {
    return this->lief_extractor->write_elf_output(output_path, this->image_base, this->functions, this->stripped);
}