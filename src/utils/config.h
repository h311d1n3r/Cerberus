#ifndef CERBERUS_CONFIG_H
#define CERBERUS_CONFIG_H

#include <string>
#include <cstdint>

struct CONFIG {
    std::string binary_path;
    std::string output_path;
    uint16_t part_hash_len = 20;
    float part_hash_trust = 0.6;
    uint16_t min_func_size = 10;
    bool debug = false;
    bool no_prompt = false;
};

#endif //CERBERUS_CONFIG_H
