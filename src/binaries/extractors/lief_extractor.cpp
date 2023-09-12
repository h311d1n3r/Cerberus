#include <binaries/extractors/lief_extractor.h>
#include <utils/convert.h>

using namespace std;

LiefExtractor::LiefExtractor(std::string bin_path) : BinaryExtractor(bin_path) {
    this->bin = LIEF::Parser::parse(bin_path);
}

BIN_ARCH LiefExtractor::extract_arch() {
    switch(this->bin->header().architecture()) {
        case LIEF::ARCH_X86:
            if(this->bin->header().is_32()) return BIN_ARCH::X86;
            return BIN_ARCH::X86_64;
    }
    return BIN_ARCH::UNKNOWN_ARCH;
}

vector<unique_ptr<FUNCTION>> LiefExtractor::extract_functions() {
    vector<unique_ptr<FUNCTION>> funcs;
    for(LIEF::Function lief_func : this->bin->exported_functions()) {
        unique_ptr<FUNCTION> func = make_unique<FUNCTION>();
        func->start = lief_func.address();
        func->end = lief_func.address() + lief_func.size();
        func->name = demangle_function_name(lief_func.name());
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