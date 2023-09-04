#include <binaries/extractors/lief_extractor.h>

LiefExtractor::LiefExtractor(std::string bin_path) : BinaryExtractor(bin_path) {
    this->bin = LIEF::Parser::parse(bin_path);
}

std::vector<FUNCTION*> LiefExtractor::extract_functions() {
    std::vector<FUNCTION*> funcs;
    for(LIEF::Function lief_func : this->bin->exported_functions()) {
        FUNCTION* func = new FUNCTION;
        func->start = lief_func.address();
        func->end = lief_func.address() + lief_func.size();
        funcs.push_back(func);
    }
    return funcs;
}

std::vector<SECTION*> LiefExtractor::extract_sections() {
    std::vector<SECTION*> sections;
    for(LIEF::Section lief_section : this->bin->sections()) {
        SECTION* section = new SECTION;
        section->start = lief_section.offset();
        section->end = lief_section.offset() + lief_section.size();
        section->name = lief_section.name();
        sections.push_back(section);
    }
    return sections;
}