#ifndef CERBERUS_PE_HANDLER_H
#define CERBERUS_PE_HANDLER_H

#include <binaries/bin_handler.h>

class PeHandler : public BinaryHandler {
private:
    size_t extract_section_header_start(std::string section_name, std::string path);
    void fix_functions_names(std::vector<std::unique_ptr<FUNCTION>>& funcs, std::string path, LiefExtractor& extractor);
public:
    PeHandler(std::string bin_path, std::string work_dir, LANG lang, Algorithm* algorithm) : BinaryHandler(bin_path, work_dir, lang, algorithm, BIN_TYPE::PE) {}
    void strip_analysis() override;
    size_t functions_analysis() override;
    void functions_matching(std::string lib_path) override;
    void post_matching() override;
    bool write_output(std::string output_path) override;
};

#endif //CERBERUS_PE_HANDLER_H
