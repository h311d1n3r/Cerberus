#ifndef CERBERUS_RADARE_EXTRACTOR_H
#define CERBERUS_RADARE_EXTRACTOR_H

#include <binaries/bin_extractor.h>
#include <command/command_executor.h>

class RadareExtractor : public BinaryExtractor {
private:
    CommandExecutor executor;
public:
    RadareExtractor(std::string bin_path) : BinaryExtractor(bin_path), executor("./") {}
    std::vector<FUNCTION*> extract_functions() override;
    std::vector<SECTION*> extract_sections() override;
};

#endif //CERBERUS_RADARE_EXTRACTOR_H
