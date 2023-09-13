#include <algorithm/part_hash_algorithm.h>
#include <utils/convert.h>
#include <iostream>

using namespace std;

void PartHashAlgorithm::compute_part_hash(unique_ptr<FUNCTION>& func, ifstream* file) {
    char data[func->end - func->start + 1];
    file->seekg(func->start);
    file->read(data, sizeof(data));
    float pace = sizeof(data) / (float)config->part_hash_len;
    if(pace == 0) pace = 1;
    func->hash = "";
    for(float i = 0; i < sizeof(data); i+=pace) {
        func->hash += data[(uint32_t)i];
    }
    for(uint8_t i = func->hash.size(); i < config->part_hash_len; i++) func->hash+='\x00';
    func->hash = func->hash.substr(0, config->part_hash_len);
}

float PartHashAlgorithm::compare_part_hashes(string hash1, string hash2) {
    uint8_t score = 0;
    for(uint8_t i = 0; i < hash1.size(); i++) {
        if(hash1.at(i) == hash2.at(i)) score++;
    }
    return score / (float) config->part_hash_len;
}

void PartHashAlgorithm::process_binary(vector<unique_ptr<FUNCTION>>* bin_funcs) {
    for(unique_ptr<FUNCTION>& func : *bin_funcs) {
        func->name = "";
        if(func->end - func->start + 1 >= config->min_func_size) this->compute_part_hash(func, bin_file);
        funcs_by_sz[func->end - func->start + 1].push_back(move(func));
    }
}

void PartHashAlgorithm::process_lib(string lib_path, vector<unique_ptr<FUNCTION>>* lib_funcs) {
    ifstream lib_file(lib_path, ios::binary);
    for (unique_ptr<FUNCTION>& lib_func : *lib_funcs) {
        size_t lib_func_sz = lib_func->end - lib_func->start + 1;
        if(lib_func_sz < config->min_func_size) continue;
        for(unique_ptr<FUNCTION>& func : funcs_by_sz[lib_func_sz]) {
            this->compute_part_hash(lib_func, &lib_file);
            float score = compare_part_hashes(lib_func->hash, func->hash);
            if (score >= config->part_hash_trust) {
                if(func->score < score) {
                    func->name = lib_func->name;
                    func->score = score;
                }
            }
        }
    }
    lib_file.close();
}

void PartHashAlgorithm::post_process(vector<unique_ptr<FUNCTION>>* bin_funcs) {
    bin_funcs->clear();
    for(auto& f_pair : funcs_by_sz) {
        for(unique_ptr<FUNCTION>& f : f_pair.second) bin_funcs->push_back(move(f));
    }
}