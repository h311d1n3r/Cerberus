#ifndef CERBERUS_BIN_HANDLER_H
#define CERBERUS_BIN_HANDLER_H

#include <string>
#include <vector>
#include <binaries/bin_types.h>
#include <binaries/extractors/lief_extractor.h>
#include <binaries/extractors/radare_extractor.h>
#include <langs/lang_types.h>

class BinaryHandler {
protected:
    std::string bin_path;
    bool stripped;
    std::vector<LIBRARY*> libs;
    std::vector<FUNCTION*> functions;
    LiefExtractor* lief_extractor;
    RadareExtractor* radare_extractor;
public:
    BinaryHandler(std::string bin_path) : bin_path(bin_path) {
        this->lief_extractor = new LiefExtractor(bin_path);
        this->radare_extractor = new RadareExtractor(bin_path);
    }
    virtual void strip_analysis() = 0;
    size_t libs_extraction(LANG type);
    virtual size_t libs_installation() = 0;
    virtual void libs_analysis() = 0;
    virtual size_t functions_analysis() = 0;
    void function_hashing(FUNCTION* func);
    virtual size_t functions_matching() = 0;
    bool is_stripped() {
        return this->stripped;
    }
    std::vector<LIBRARY> get_libs() {
        return this->libs;
    }
    std::vector<FUNCTION> get_functions() {
        return this->functions;
    }
};

#endif //CERBERUS_BIN_HANDLER_H
