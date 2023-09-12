#ifndef CERBERUS_ELF_HANDLER_H
#define CERBERUS_ELF_HANDLER_H

#include <binaries/bin_handler.h>

class ElfHandler : public BinaryHandler {
public:
    ElfHandler(std::string bin_path, std::string work_dir, LANG lang, Algorithm* algorithm) : BinaryHandler(bin_path, work_dir, lang, algorithm) {}
    void strip_analysis() override;
    size_t functions_analysis() override;
    void functions_matching(std::string lib_path) override;
};

#endif //CERBERUS_ELF_HANDLER_H
