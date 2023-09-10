#ifndef CERBERUS_BIN_HANDLER_H
#define CERBERUS_BIN_HANDLER_H

#include <string>
#include <vector>
#include <binaries/bin_types.h>
#include <binaries/extractors/lief_extractor.h>
#include <binaries/extractors/radare_extractor.h>
#include <langs/lang_types.h>
#include <user/dependencies/dependency_manager.h>

class BinaryHandler {
protected:
    std::string bin_path;
    std::string work_dir;
    LANG lang;
    bool stripped;
    std::vector<LIBRARY*> libs;
    std::vector<FUNCTION*> functions;
    std::vector<PACKAGE*> packages;
    LiefExtractor* lief_extractor;
    RadareExtractor* radare_extractor;
public:
    BinaryHandler(std::string bin_path, std::string work_dir, LANG lang) : bin_path(bin_path), work_dir(work_dir), lang(lang) {
        this->packages.push_back(new OS_PACKAGE{"git", "git"});
        this->packages.push_back(new GIT_PACKAGE{"radare2", "radare2", "https://github.com/radareorg/radare2", 0, "cd .. ; mv radare2 ../ ; ../radare2/sys/install.sh"});
        this->lief_extractor = new LiefExtractor(bin_path);
        this->radare_extractor = new RadareExtractor(bin_path);
    }
    virtual void strip_analysis() = 0;
    size_t libs_extraction();
    size_t libs_installation();
    virtual void libs_analysis() = 0;
    virtual size_t functions_analysis() = 0;
    void function_hashing(FUNCTION* func);
    virtual size_t functions_matching() = 0;
    bool is_stripped() {
        return this->stripped;
    }
    std::vector<LIBRARY*> get_libs() {
        return this->libs;
    }
    std::vector<FUNCTION*> get_functions() {
        return this->functions;
    }
    std::vector<PACKAGE*> get_packages() {
        return packages;
    }
};

#endif //CERBERUS_BIN_HANDLER_H
