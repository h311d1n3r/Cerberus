#include <binaries/extractors/lief_extractor.h>
#include <utils/convert.h>
#include <binaries/pe_types.h>
#include <fstream>
#include <filesystem>

using namespace std;

LiefExtractor::LiefExtractor(std::string bin_path, BIN_TYPE type) : BinaryExtractor(bin_path, type) {
    this->bin = LIEF::Parser::parse(bin_path);
    switch(type) {
        case BIN_TYPE::ELF:
            this->elf_bin = LIEF::ELF::Parser::parse(bin_path);
            break;
        case BIN_TYPE::PE:
            this->pe_bin = LIEF::PE::Parser::parse(bin_path);
            break;
    }
}

BIN_ARCH LiefExtractor::extract_arch() {
    switch(this->bin->header().architecture()) {
        case LIEF::ARCH_X86:
            if(this->bin->header().is_32()) return BIN_ARCH::X86;
            return BIN_ARCH::X86_64;
    }
    return BIN_ARCH::UNKNOWN_ARCH;
}

size_t LiefExtractor::extract_image_base() {
    return this->bin->imagebase();
}

size_t LiefExtractor::resolve_pe_rva(size_t rva) {
    return this->pe_bin->rva_to_offset(rva);
}

LIEF::PE::Section* LiefExtractor::extract_text_section() {
    return this->pe_bin->get_section(".text");
}

vector<unique_ptr<FUNCTION>> LiefExtractor::extract_functions(BIN_ARCH arch, size_t image_base) {
    if(type == BIN_TYPE::PE) image_base = 0;
    vector<unique_ptr<FUNCTION>> funcs;
    vector<LIEF::Function> lief_funcs;
    switch(type) {
        case BIN_TYPE::ELF:
            lief_funcs = this->elf_bin->functions();
            break;
        case BIN_TYPE::PE:
            lief_funcs = this->pe_bin->functions();
            break;
    }
    size_t ordinal = 0;
    for(LIEF::Function lief_func : lief_funcs) {
        if(!lief_func.size()) continue;
        unique_ptr<FUNCTION> func = make_unique<FUNCTION>();
        switch(type) {
            case BIN_TYPE::ELF:
                func->start = lief_func.address() - image_base;
                break;
            case BIN_TYPE::PE:
                func->start = resolve_pe_rva(lief_func.address());
                break;
        }
        func->end = func->start + lief_func.size() - 1;
        func->name = lief_func.name();
        func->ordinal = ordinal;
        funcs.push_back(move(func));
        ordinal++;
    }
    return funcs;
}

vector<unique_ptr<SECTION>> LiefExtractor::extract_sections() {
    vector<unique_ptr<SECTION>> sections;
    for(LIEF::Section lief_section : this->bin->sections()) {
        unique_ptr<SECTION> section = make_unique<SECTION>();
        section->start = lief_section.offset();
        section->end = lief_section.offset() + lief_section.size();
        section->name = lief_section.name();
        sections.push_back(move(section));
    }
    return sections;
}

bool LiefExtractor::write_elf_output(string output_path, size_t image_base, vector<unique_ptr<FUNCTION>>& funcs, bool stripped) {
    if(!this->elf_bin->has_section(".symtab")) {
        LIEF::ELF::Section symtab_sec = LIEF::ELF::Section();
        symtab_sec.name(".symtab");
        symtab_sec.type(LIEF::ELF::ELF_SECTION_TYPES::SHT_SYMTAB);
        symtab_sec.entry_size(0x18);
        symtab_sec.alignment(8);
        symtab_sec.link(this->elf_bin->header().numberof_sections()+1);
        vector<uint8_t> content(100, 0);
        symtab_sec.content(content);
        *this->elf_bin->add(symtab_sec, false);
    }
    if(!this->elf_bin->has_section(".strtab")) {
        LIEF::ELF::Section strtab_sec = LIEF::ELF::Section();
        strtab_sec.name(".strtab");
        strtab_sec.type(LIEF::ELF::ELF_SECTION_TYPES::SHT_STRTAB);
        strtab_sec.entry_size(1);
        strtab_sec.alignment(1);
        vector<uint8_t> content(100, 0);
        strtab_sec.content(content);
        *this->elf_bin->add(strtab_sec, false);
    }
    vector<uint64_t> sym_addresses;
    if(stripped) {
        LIEF::ELF::Symbol symbol = LIEF::ELF::Symbol();
        symbol.name("");
        symbol.type(LIEF::ELF::ELF_SYMBOL_TYPES::STT_NOTYPE);
        symbol.value(0);
        symbol.binding(LIEF::ELF::SYMBOL_BINDINGS::STB_LOCAL);
        symbol.size(0);
        symbol.shndx(0);
        this->elf_bin->add_static_symbol(symbol);
    } else {
        for(auto& symbol : this->elf_bin->symbols()) {
            uint64_t sym_addr = symbol.value();
            if(find(sym_addresses.begin(), sym_addresses.end(), sym_addr) == sym_addresses.end()) sym_addresses.push_back(sym_addr);
        }
    }
    for(unique_ptr<FUNCTION>& func : funcs) {
        if(stripped || std::find(sym_addresses.begin(), sym_addresses.end(), func->start) == sym_addresses.end()) {
            LIEF::ELF::Symbol symbol = LIEF::ELF::Symbol();
            symbol.name(func->name);
            symbol.type(LIEF::ELF::ELF_SYMBOL_TYPES::STT_FUNC);
            symbol.value(func->start+image_base);
            symbol.binding(LIEF::ELF::SYMBOL_BINDINGS::STB_LOCAL);
            symbol.shndx(14);
            this->elf_bin->add_static_symbol(symbol);
        }
    }
    this->elf_bin->write(output_path);
    return true;
}

bool LiefExtractor::write_pe_output(string output_path, size_t image_base, size_t matches_sz, vector<unique_ptr<FUNCTION>>& funcs) {
    vector<uint8_t> edata_content;
    EXPORT_DIRECTORY_TABLE edt;
    for(size_t i = 0; i < sizeof(edt) + matches_sz * 8; i++) edata_content.push_back(0);
    for(uint16_t i = 0; i < matches_sz; i++) {
        edata_content.push_back(i & 0xff);
        edata_content.push_back(i >> 8);
    }
    for(unique_ptr<FUNCTION>& func : funcs) {
        if(!func->name.size()) continue;
        for(char c : func->name) edata_content.push_back(c);
        edata_content.push_back(0);
    }
    LIEF::PE::Section edata_section;
    edata_section.name(".edata");
    edata_section.content(edata_content);
    edata_section.characteristics((uint32_t)(
            LIEF::PE::SECTION_CHARACTERISTICS::IMAGE_SCN_CNT_INITIALIZED_DATA |
            LIEF::PE::SECTION_CHARACTERISTICS::IMAGE_SCN_MEM_READ
        ) | 0x300000);
    edata_section = *this->pe_bin->add_section(edata_section);
    this->pe_bin->header().numberof_sections(this->pe_bin->header().numberof_sections() + 1);
    this->pe_bin->write(output_path);
    ifstream bin_file(output_path, ios::binary);
    bin_file.seekg(0, ios::end);
    size_t bin_file_sz = bin_file.tellg();
    ofstream new_bin_file(output_path+".tmp", ios::binary);
    size_t edata_start = edata_section.virtual_address();
    edt.name_rva = edata_start + matches_sz * 10;
    uint32_t edata_content_sz = edata_content.size();
    edt.address_table_entries = matches_sz;
    edt.number_of_name_pointers = matches_sz;
    edt.export_address_table_rva = edata_start + sizeof(edt);
    edt.name_pointer_rva = edata_start + sizeof(edt) + 4 * matches_sz;
    edt.ordinal_table_rva = edata_start + sizeof(edt) + 8 * matches_sz;
    for(uint8_t i = 0; i < sizeof(edt); i++) edata_content[i] = (((char*)&edt)[i]);
    size_t func_i = 0;
    size_t name_pos = 0;
    for(unique_ptr<FUNCTION>& func : funcs) {
        if(!func->name.size()) continue;
        uint32_t func_vaddr = this->pe_bin->offset_to_virtual_address(func->start).value();
        memcpy((char*)(edata_content.data()+sizeof(edt)+func_i*4), &func_vaddr, sizeof(uint32_t));
        uint32_t name_vaddr = edata_start + sizeof(edt) + matches_sz * 10 + name_pos;
        memcpy((char*)(edata_content.data()+sizeof(edt)+matches_sz*4+func_i*4), &name_vaddr, sizeof(uint32_t));
        func_i++;
        name_pos += func->name.size()+1;
    }
    char buffer[1024];
    uint32_t pe_start;
    bin_file.seekg(0x3c);
    bin_file.read((char*)&pe_start, sizeof(pe_start));
    size_t edt_data_dir = pe_start + 0x88;
    bin_file.seekg(0, ios::beg);
    bin_file.read(buffer, edt_data_dir);
    new_bin_file.write(buffer, edt_data_dir);
    new_bin_file.write((char*)&edata_start, 4);
    new_bin_file.write((char*)&edata_content_sz, 4);
    bin_file.seekg((size_t)bin_file.tellg()+8);
    size_t off = edt_data_dir+0x8;
    while(off < edata_section.offset() - sizeof(buffer)) {
        bin_file.read(buffer, sizeof(buffer));
        new_bin_file.write(buffer, sizeof(buffer));
        off+=sizeof(buffer);
    }
    bin_file.read(buffer, edata_section.offset() - off);
    new_bin_file.write(buffer, edata_section.offset() - off);
    new_bin_file.write((char*)edata_content.data(), edata_content.size());
    bin_file.seekg((size_t)bin_file.tellg()+edata_content.size());
    off = edata_section.offset() + edata_content.size();
    while(off < bin_file_sz - sizeof(buffer)) {
        bin_file.read(buffer, sizeof(buffer));
        new_bin_file.write(buffer, sizeof(buffer));
        off+=sizeof(buffer);
    }
    new_bin_file.write(buffer, bin_file_sz - off);
    bin_file.close();
    new_bin_file.close();
    filesystem::rename(output_path+".tmp", output_path);
    return true;
}