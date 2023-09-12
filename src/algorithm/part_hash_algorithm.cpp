#include <algorithm/part_hash_algorithm.h>
#include <utils/convert.h>
#include <iostream>

using namespace std;

string PartHashAlgorithm::compute_part_hash(unique_ptr<FUNCTION>& func, ifstream* file) {
    char data[func->end - func->start + 1];
    file->seekg(func->start);
    file->read(data, sizeof(data));
    float pace = sizeof(data) / (float)config->part_hash_len;
    if(pace == 0) pace = 1;
    string hash;
    for(float i = 0; i < sizeof(data); i+=pace) {
        hash += data[(uint32_t)i];
    }
    hash = hash.substr(0, sizeof(data));
    return hash;
}

uint8_t PartHashAlgorithm::compare_part_hashes(string hash1, string hash2) {
    uint8_t score = 0;
    for(uint8_t i = 0; i < hash1.size(); i++) {
        if(hash1.at(i) == hash2.at(i)) score++;
    }
    return score;
}

void PartHashAlgorithm::process_binary(vector<unique_ptr<FUNCTION>>* bin_funcs) {
    for(unique_ptr<FUNCTION>& func : *bin_funcs) {
        func->name = "";
        if(func->end - func->start + 1 < config->min_func_size) continue;
        func->hash = this->compute_part_hash(func, bin_file);
        funcs_by_sz[func->end - func->start + 1].push_back(func.get());
    }
}

void PartHashAlgorithm::process_lib(string lib_path, vector<unique_ptr<FUNCTION>>* lib_funcs) {
    cout << lib_path << endl;
    ifstream lib_file(lib_path, ios::binary);
    for (unique_ptr<FUNCTION>& lib_func : *lib_funcs) {
        size_t lib_func_sz = lib_func->end - lib_func->start + 1;
        for(FUNCTION* func : funcs_by_sz[lib_func_sz]) {
            string hash = this->compute_part_hash(lib_func, &lib_file);
            uint8_t score = compare_part_hashes(hash, func->hash);
            cout << +score / (float) hash.size() << endl;
            if (score / (float) hash.size() >= config->part_hash_trust) {
                cout << "OK" << endl;
            }
        }
    }
    lib_file.close();
}