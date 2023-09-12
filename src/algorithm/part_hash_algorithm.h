#ifndef CERBERUS_PART_HASH_ALGORITHM_H
#define CERBERUS_PART_HASH_ALGORITHM_H

#include <algorithm/algorithm.h>
#include <map>
#include <memory>

class PartHashAlgorithm : public Algorithm {
private:
    std::map<size_t, std::vector<FUNCTION*>> funcs_by_sz;
    std::string compute_part_hash(std::unique_ptr<FUNCTION>& func, std::ifstream* file);
    uint8_t compare_part_hashes(std::string hash1, std::string hash2);
public:
    PartHashAlgorithm(CONFIG* config) : Algorithm(config) {}
    void process_binary(std::vector<std::unique_ptr<FUNCTION>>* bin_funcs) override;
    void process_lib(std::string lib_path, std::vector<std::unique_ptr<FUNCTION>>* lib_funcs) override;
};

#endif //CERBERUS_PART_HASH_ALGORITHM_H
