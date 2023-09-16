#ifndef CERBERUS_RADARE_EXTRACTOR_H
#define CERBERUS_RADARE_EXTRACTOR_H

#include <binaries/bin_extractor.h>
#include <command/command_executor.h>

class RadareExtractor : public BinaryExtractor {
private:
    CommandExecutor executor;
public:
    RadareExtractor(std::string bin_path) : BinaryExtractor(bin_path), executor("./") {}
    BIN_ARCH extract_arch() override;
    std::vector<std::unique_ptr<FUNCTION>> extract_functions(BIN_ARCH arch, size_t image_base) override;
    std::vector<std::unique_ptr<SECTION>> extract_sections() override;
};

#endif //CERBERUS_RADARE_EXTRACTOR_H
