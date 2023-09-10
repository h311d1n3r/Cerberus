#ifndef CERBERUS_PE_HANDLER_H
#define CERBERUS_PE_HANDLER_H

#include <binaries/bin_handler.h>

class PeHandler : public BinaryHandler {
public:
    PeHandler(std::string bin_path, std::string work_dir, LANG lang) : BinaryHandler(bin_path, work_dir, lang) {}
    void strip_analysis() override;
    size_t functions_analysis() override;
    void libs_analysis() override;
    size_t functions_matching() override;
};

#endif //CERBERUS_PE_HANDLER_H
