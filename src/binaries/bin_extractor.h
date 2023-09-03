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
    virtual std::vector<FUNCTION> extract_functions();
    virtual std::vector<SECTION> extract_sections();
};

#endif //CERBERUS_BIN_EXTRACTOR_H
