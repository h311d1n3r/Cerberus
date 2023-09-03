#ifndef CERBERUS_BIN_HANDLER_H
#define CERBERUS_BIN_HANDLER_H

#include <string>
#include <vector>
#include <binaries/bin_types.h>

class BinaryHandler {
protected:
    std::string bin_path;
    bool stripped;
    std::vector<LIBRARY> libs;
    std::vector<FUNCTION> functions;
public:
    BinaryHandler(std::string bin_path) : bin_path(bin_path) {}
    virtual void strip_analysis();
    virtual void libs_analysis();
    virtual size_t libs_installation();
    virtual size_t functions_analysis();
    virtual void functions_hashing();
    virtual size_t functions_matching();
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
