#include <binaries/handlers/pe_handler.h>

using namespace std;

void PeHandler::strip_analysis() {
    this->stripped = !this->extract_section_header_start(".edata", this->bin_path);
}

size_t PeHandler::functions_analysis() {
    vector<unique_ptr<FUNCTION>> funcs = this->lief_extractor->extract_functions(this->arch, this->image_base);
    if(!funcs.size()) funcs = this->radare_extractor->extract_functions(this->arch, this->image_base);
    algorithm->process_binary(&funcs);
    this->functions = move(funcs);
    return this->functions.size();
}

size_t PeHandler::extract_section_header_start(std::string section_name, std::string path) {
    ifstream bin_file(path);
    uint32_t pe_start;
    bin_file.seekg(0x3c);
    bin_file.read((char*)&pe_start, sizeof(pe_start));
    uint16_t number_of_sections;
    bin_file.seekg(pe_start+0x6);
    bin_file.read((char*)&number_of_sections, sizeof(number_of_sections));
    uint16_t size_of_opt_header;
    bin_file.seekg(pe_start+0x14);
    bin_file.read((char*)&size_of_opt_header, sizeof(size_of_opt_header));
    for(uint16_t sec_i = 0; sec_i < number_of_sections; sec_i++) {
        size_t sec_start = pe_start + 0x18 + size_of_opt_header + sec_i * 0x28;
        bin_file.seekg(sec_start);
        char sec_name[8];
        bin_file.read(sec_name, sizeof(sec_name));
        if(!strcmp(sec_name, section_name.c_str())) {
            bin_file.close();
            return sec_start;
        }
    }
    bin_file.close();
    return 0;
}

void PeHandler::fix_functions_names(vector<unique_ptr<FUNCTION>>& funcs, std::string path, LiefExtractor& extractor) {
    size_t edata_header_start = this->extract_section_header_start(".edata", path);
    if(!edata_header_start) return;
    ifstream bin_file(path);
    uint32_t edata_start;
    bin_file.seekg(edata_header_start+0xc);
    bin_file.read((char*)&edata_start, sizeof(edata_start));
    edata_start = extractor.resolve_pe_rva(edata_start);
    uint32_t pointers_sz;
    bin_file.seekg(edata_start+0x18);
    bin_file.read((char*)&pointers_sz, sizeof(pointers_sz));
    uint32_t eat_start;
    bin_file.seekg(edata_start+0x1c);
    bin_file.read((char*)&eat_start, sizeof(eat_start));
    eat_start = extractor.resolve_pe_rva(eat_start);
    uint32_t name_table_start;
    bin_file.seekg(edata_start+0x20);
    bin_file.read((char*)&name_table_start, sizeof(name_table_start));
    name_table_start = extractor.resolve_pe_rva(name_table_start);
    map<size_t, string> eat;
    for(size_t entry_i = 0; entry_i < pointers_sz; entry_i++) {
        size_t eat_entry = eat_start + entry_i * sizeof(uint32_t);
        uint32_t func_addr;
        bin_file.seekg(eat_entry);
        bin_file.read((char*)&func_addr, sizeof(func_addr));
        func_addr = extractor.resolve_pe_rva(func_addr);
        size_t name_entry = name_table_start + entry_i * sizeof(uint32_t);
        uint32_t name_addr;
        bin_file.seekg(name_entry);
        bin_file.read((char*)&name_addr, sizeof(name_addr));
        name_addr = extractor.resolve_pe_rva(name_addr);
        char name[1024];
        bin_file.seekg(name_addr);
        bin_file.read(name, sizeof(name));
        eat[func_addr] = string(name);
    }
    for(unique_ptr<FUNCTION>& func : funcs) {
        if(func->name.length()) continue;
        if(eat.find(func->start) != eat.end()) {
            func->name = eat[func->start];
        }
    }
    bin_file.close();
}

void PeHandler::functions_matching(string lib_path) {
    LiefExtractor lib_lief_extractor(lib_path, type);
    RadareExtractor lib_radare_extractor(lib_path);
    size_t lib_image_base = lib_lief_extractor.extract_image_base();
    vector<unique_ptr<FUNCTION>> funcs = lib_lief_extractor.extract_functions(this->arch, lib_image_base);
    if(!funcs.size()) funcs = lib_radare_extractor.extract_functions(this->arch, lib_image_base);
    this->fix_functions_names(funcs, lib_path, lib_lief_extractor);
    algorithm->process_lib(lib_path, &funcs);
}

void PeHandler::post_matching() {
    algorithm->post_process(&functions);
}

bool PeHandler::write_output(string output_path) {
    return this->lief_extractor->write_pe_output(output_path, this->image_base, this->matches_sz, this->functions);
}