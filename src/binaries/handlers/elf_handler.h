#ifndef CERBERUS_ELF_HANDLER_H
#define CERBERUS_ELF_HANDLER_H

#include <binaries/bin_handler.h>

class ElfHandler : public BinaryHandler {
public:
    void strip_analysis();
    void libs_analysis();
    size_t libs_installation();
    size_t functions_analysis();
    void functions_hashing();
    size_t functions_matching();
};

#endif //CERBERUS_ELF_HANDLER_H
