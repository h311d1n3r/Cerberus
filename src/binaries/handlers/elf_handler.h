#ifndef CERBERUS_ELF_HANDLER_H
#define CERBERUS_ELF_HANDLER_H

#include <binaries/bin_handler.h>

class ElfHandler : public BinaryHandler {
private:
    LibelfExtractor* libelf_extractor;
public:
    ElfHandler(std::string bin_path, std::string work_dir, LANG lang, Algorithm* algorithm) : BinaryHandler(bin_path, work_dir, lang, algorithm, BIN_TYPE::ELF) {
        this->libelf_extractor = new LibelfExtractor(bin_path, type);
    }
    void strip_analysis() override;
    size_t functions_analysis() override;
    void functions_matching(std::string lib_path) override;
    void post_matching() override;
    bool write_output(std::string output_path) override;
};

#endif //CERBERUS_ELF_HANDLER_H
