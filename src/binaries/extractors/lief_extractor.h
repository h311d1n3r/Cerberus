#ifndef CERBERUS_LIEF_EXTRACTOR_H
#define CERBERUS_LIEF_EXTRACTOR_H

#include <binaries/bin_extractor.h>
#include <LIEF/LIEF.hpp>

class LiefExtractor : public BinaryExtractor {
private:
    std::unique_ptr<LIEF::Binary> bin;
    std::unique_ptr<LIEF::ELF::Binary> elf_bin;
    std::unique_ptr<LIEF::PE::Binary> pe_bin;
    BIN_TYPE type;
public:
    LiefExtractor(std::string bin_path, BIN_TYPE type);
    BIN_ARCH extract_arch() override;
    size_t extract_image_base();
    size_t resolve_pe_rva(size_t rva);
    std::vector<std::unique_ptr<FUNCTION>> extract_functions(BIN_ARCH arch, size_t image_base) override;
    std::vector<std::unique_ptr<SECTION>> extract_sections() override;
    bool write_elf_output(std::string output_path, size_t image_base, std::vector<std::unique_ptr<FUNCTION>>& funcs, bool stripped);
    bool write_pe_output(std::string output_path, size_t image_base, size_t matches_sz, std::vector<std::unique_ptr<FUNCTION>>& funcs);
};

#endif //CERBERUS_LIEF_EXTRACTOR_H
