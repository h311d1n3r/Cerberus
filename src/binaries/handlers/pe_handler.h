#ifndef CERBERUS_PE_HANDLER_H
#define CERBERUS_PE_HANDLER_H

#include <binaries/bin_handler.h>

class PeHandler : public BinaryHandler {
public:
    PeHandler(std::string bin_path, std::string work_dir, LANG lang, Algorithm* algorithm) : BinaryHandler(bin_path, work_dir, lang, algorithm) {}
    void strip_analysis() override;
    size_t functions_analysis() override;
    void functions_matching(std::string lib_path) override;
    void post_matching() override;
};

#endif //CERBERUS_PE_HANDLER_H
