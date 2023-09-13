#ifndef CERBERUS_ALGORITHM_H
#define CERBERUS_ALGORITHM_H

#include <vector>
#include <fstream>
#include <binaries/bin_types.h>
#include <utils/config.h>
#include <memory>

class Algorithm {
protected:
    std::ifstream* bin_file;
    CONFIG* config;
public:
    Algorithm(CONFIG* config) : config(config) {
        bin_file = new std::ifstream(config->binary_path, std::ios::binary);
    }
    ~Algorithm() {
        bin_file->close();
    }
    virtual void process_binary(std::vector<std::unique_ptr<FUNCTION>>* bin_funcs) = 0;
    virtual void process_lib(std::string lib_path, std::vector<std::unique_ptr<FUNCTION>>* lib_funcs) = 0;
    virtual void post_process(std::vector<std::unique_ptr<FUNCTION>>* bin_funcs) = 0;
};

#endif //CERBERUS_ALGORITHM_H
