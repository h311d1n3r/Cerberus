#ifndef CERBERUS_ELF_HANDLER_H
#define CERBERUS_ELF_HANDLER_H

#include <binaries/bin_handler.h>

class ElfHandler : public BinaryHandler {
public:
    ElfHandler(std::string bin_path) : BinaryHandler(bin_path) {}
    void strip_analysis() override;
    size_t libs_installation() override;
    void libs_analysis() override;
    size_t functions_analysis() override;
    size_t functions_matching() override;
};

#endif //CERBERUS_ELF_HANDLER_H
