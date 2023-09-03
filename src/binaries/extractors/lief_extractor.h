#ifndef CERBERUS_LIEF_EXTRACTOR_H
#define CERBERUS_LIEF_EXTRACTOR_H

#include <binaries/bin_extractor.h>

class LiefExtractor : public BinaryExtractor {
public:
    std::vector<FUNCTION> extract_functions();
    std::vector<SECTION> extract_sections();
};

#endif //CERBERUS_LIEF_EXTRACTOR_H
