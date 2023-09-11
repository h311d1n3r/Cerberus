#include <algorithm/part_hash_algorithm.h>
#include <iostream>

using namespace std;

string PartHashAlgorithm::compute_part_hash(FUNCTION* func) {
    char data[func->end - func->start + 1];
    bin_file->seekg(func->start);
    bin_file->read(data, sizeof(data));
    float pace = sizeof(data) / (float)config->part_hash_len;
    if(pace == 0) pace = 1;
    string hash;
    for(float i = 0; i < sizeof(data); i+=pace) {
        hash += data[(uint32_t)i];
    }
    hash = hash.substr(0, 20);
    return hash;
}

void PartHashAlgorithm::process_binary(vector<FUNCTION*>* bin_funcs) {
    this->bin_funcs = bin_funcs;
    for(FUNCTION* func : *bin_funcs) {
        func->name = "";
        if(func->end - func->start + 1 < config->min_func_size) continue;
        string hash = this->compute_part_hash(func);
    }
}

void PartHashAlgorithm::process_lib(string lib_path, vector<FUNCTION*>* lib_funcs) {
    
}