#ifndef CERBERUS_GO_EXTRACTOR_H
#define CERBERUS_GO_EXTRACTOR_H

#include <binaries/bin_extractor.h>
#include <command/command_executor.h>
#include <binaries/extractors/lief_extractor.h>

class GoExtractor : public BinaryExtractor {
private:
    LiefExtractor& lief_extractor;
    CommandExecutor executor;
public:
    GoExtractor(std::string bin_path, BIN_TYPE type, LiefExtractor& lief_extractor) : BinaryExtractor(bin_path, type), lief_extractor(lief_extractor), executor("./") {}
    BIN_ARCH extract_arch() override;
    std::vector<std::unique_ptr<FUNCTION>> extract_functions(BIN_ARCH arch, size_t image_base) override;
    std::vector<std::unique_ptr<SECTION>> extract_sections() override;
};


#endif //CERBERUS_GO_EXTRACTOR_H
