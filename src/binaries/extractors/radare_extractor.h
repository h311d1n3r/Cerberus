#ifndef CERBERUS_RADARE_EXTRACTOR_H
#define CERBERUS_RADARE_EXTRACTOR_H

#include <binaries/bin_extractor.h>

class RadareExtractor : public BinaryExtractor {
public:
    std::vector<FUNCTION> extract_functions();
    std::vector<SECTION> extract_sections();
};

#endif //CERBERUS_RADARE_EXTRACTOR_H
