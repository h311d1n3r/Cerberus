#ifndef CERBERUS_BIN_HANDLER_H
#define CERBERUS_BIN_HANDLER_H

#include <string>
#include <vector>
#include <binaries/bin_types.h>
#include <binaries/extractors/lief_extractor.h>
#include <binaries/extractors/radare_extractor.h>
#include <langs/lang_types.h>
#include <user/dependencies/dependency_manager.h>
#include <algorithm/algorithm.h>
#include <binaries/extractors/libelf_extractor.h>

class BinaryHandler {
protected:
    std::string bin_path;
    std::string work_dir;
    LANG lang;
    Algorithm* algorithm;
    BIN_ARCH arch;
    BIN_TYPE type;
    size_t image_base;
    bool stripped;
    std::vector<std::unique_ptr<LIBRARY>> libs;
    std::vector<std::unique_ptr<FUNCTION>> functions;
    size_t matches_sz = 0;
    LiefExtractor* lief_extractor;
    RadareExtractor* radare_extractor;
public:
    BinaryHandler(std::string bin_path, std::string work_dir, LANG lang, Algorithm* algorithm, BIN_TYPE type) : bin_path(bin_path), work_dir(work_dir), lang(lang), algorithm(algorithm), type(type) {
        this->lief_extractor = new LiefExtractor(bin_path, type);
        this->radare_extractor = new RadareExtractor(bin_path);
    }
    BIN_ARCH extract_architecture();
    void extract_image_base();
    virtual void strip_analysis() = 0;
    size_t libs_extraction();
    size_t libs_installation();
    virtual size_t functions_analysis() = 0;
    virtual void functions_matching(std::string lib_path) = 0;
    virtual void post_matching() = 0;
    size_t get_matches_sz();
    void demangle_functions();
    virtual bool write_output(std::string output_path) = 0;
    bool is_stripped() {
        return this->stripped;
    }
    std::vector<std::unique_ptr<LIBRARY>>& get_libs() {
        return this->libs;
    }
    std::vector<std::unique_ptr<FUNCTION>>& get_functions() {
        return this->functions;
    }
};

#endif //CERBERUS_BIN_HANDLER_H
