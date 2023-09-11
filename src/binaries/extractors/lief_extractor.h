#ifndef CERBERUS_LIEF_EXTRACTOR_H
#define CERBERUS_LIEF_EXTRACTOR_H

#include <binaries/bin_extractor.h>
#include <LIEF/LIEF.hpp>

class LiefExtractor : public BinaryExtractor {
private:
    std::unique_ptr<LIEF::Binary> bin;
public:
    LiefExtractor(std::string bin_path);
    BIN_ARCH extract_arch() override;
    std::vector<FUNCTION*> extract_functions() override;
    std::vector<SECTION*> extract_sections() override;
};

#endif //CERBERUS_LIEF_EXTRACTOR_H
