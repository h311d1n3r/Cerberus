#ifndef CERBERUS_BIN_EXTRACTOR_H
#define CERBERUS_BIN_EXTRACTOR_H

#include <string>
#include <vector>
#include <binaries/bin_types.h>
#include <memory>

class BinaryExtractor {
protected:
    std::string bin_path;
public:
    BinaryExtractor(std::string bin_path) : bin_path(bin_path) {};
    virtual BIN_ARCH extract_arch() = 0;
    virtual std::vector<std::unique_ptr<FUNCTION>> extract_functions(BIN_ARCH arch, size_t image_base) = 0;
    virtual std::vector<std::unique_ptr<SECTION>> extract_sections() = 0;
};

#endif //CERBERUS_BIN_EXTRACTOR_H
