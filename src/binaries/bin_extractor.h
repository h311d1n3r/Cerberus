#ifndef CERBERUS_BIN_EXTRACTOR_H
#define CERBERUS_BIN_EXTRACTOR_H

#include <string>
#include <vector>
#include <binaries/bin_types.h>

class BinaryExtractor {
protected:
    std::string bin_path;
public:
    BinaryExtractor(std::string bin_path) : bin_path(bin_path) {};
    virtual BIN_ARCH extract_arch() = 0;
    virtual std::vector<FUNCTION*> extract_functions() = 0;
    virtual std::vector<SECTION*> extract_sections() = 0;
};

#endif //CERBERUS_BIN_EXTRACTOR_H
