#ifndef CERBERUS_ELF_HANDLER_H
#define CERBERUS_ELF_HANDLER_H

#include <binaries/bin_handler.h>

class ElfHandler : public BinaryHandler {
public:
    ElfHandler(std::string bin_path, std::string work_dir, LANG lang) : BinaryHandler(bin_path, work_dir, lang) {}
    void strip_analysis() override;
    void libs_analysis() override;
    size_t functions_analysis() override;
    size_t functions_matching() override;
};

#endif //CERBERUS_ELF_HANDLER_H
