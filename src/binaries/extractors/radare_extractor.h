#ifndef CERBERUS_RADARE_EXTRACTOR_H
#define CERBERUS_RADARE_EXTRACTOR_H

#include <binaries/bin_extractor.h>

class RadareExtractor : public BinaryExtractor {
public:
    RadareExtractor(std::string bin_path) : BinaryExtractor(bin_path) {}
    std::vector<FUNCTION*> extract_functions() override;
    std::vector<SECTION*> extract_sections() override;
};

#endif //CERBERUS_RADARE_EXTRACTOR_H
