#include <binaries/extractors/lief_extractor.h>
#include <utils/convert.h>
#include <iostream>

using namespace std;

LiefExtractor::LiefExtractor(std::string bin_path, bool is_elf) : BinaryExtractor(bin_path), is_elf(is_elf) {
    this->bin = LIEF::Parser::parse(bin_path);
    if(is_elf) this->elf_bin = LIEF::ELF::Parser::parse(bin_path);
    else this->pe_bin = LIEF::PE::Parser::parse(bin_path);
}

BIN_ARCH LiefExtractor::extract_arch() {
    switch(this->bin->header().architecture()) {
        case LIEF::ARCH_X86:
            if(this->bin->header().is_32()) return BIN_ARCH::X86;
            return BIN_ARCH::X86_64;
    }
    return BIN_ARCH::UNKNOWN_ARCH;
}

vector<unique_ptr<FUNCTION>> LiefExtractor::extract_functions(BIN_ARCH arch) {
    vector<unique_ptr<FUNCTION>> funcs;
    vector<LIEF::Function> lief_funcs;
    if(is_elf) lief_funcs = this->elf_bin->functions();
    else lief_funcs = this->pe_bin->functions();
    for(LIEF::Function lief_func : lief_funcs) {
        if(!lief_func.size()) continue;
        unique_ptr<FUNCTION> func = make_unique<FUNCTION>();
        func->start = lief_func.address();
        func->end = lief_func.address() + lief_func.size();
        func->name = lief_func.name();
        funcs.push_back(move(func));
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

bool LiefExtractor::write_elf_output(string output_path, vector<unique_ptr<FUNCTION>>& funcs, bool stripped) {
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
            symbol.value(func->start);
            symbol.binding(LIEF::ELF::SYMBOL_BINDINGS::STB_LOCAL);
            symbol.shndx(14);
            this->elf_bin->add_static_symbol(symbol);
        }
    }
    this->elf_bin->write(output_path);
    return true;
}