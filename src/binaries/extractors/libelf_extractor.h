#ifndef CERBERUS_LIBELF_EXTRACTOR_H
#define CERBERUS_LIBELF_EXTRACTOR_H

#include <binaries/bin_extractor.h>
#include <libelf.h>

class LibelfExtractor : public BinaryExtractor {
private:
    Elf* bin;
    int32_t fd;
    FILE* fp;
public:
    LibelfExtractor(std::string bin_path);
    ~LibelfExtractor();
    BIN_ARCH extract_arch() override;
    std::vector<std::unique_ptr<FUNCTION>> extract_functions_32(size_t image_base);
    std::vector<std::unique_ptr<FUNCTION>> extract_functions_64(size_t image_base);
    std::vector<std::unique_ptr<FUNCTION>> extract_functions(BIN_ARCH arch, size_t image_base) override;
    std::vector<std::unique_ptr<SECTION>> extract_sections() override;
};

#endif //CERBERUS_LIBELF_EXTRACTOR_H
