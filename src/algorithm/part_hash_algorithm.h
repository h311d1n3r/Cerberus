#ifndef CERBERUS_PART_HASH_ALGORITHM_H
#define CERBERUS_PART_HASH_ALGORITHM_H

#include <algorithm/algorithm.h>
#include <types/value_ordered_map.h>

class PartHashAlgorithm : public Algorithm {
private:
    value_ordered_map<FUNCTION*, uint8_t> scores;
    std::string compute_part_hash(FUNCTION* func);
    std::vector<FUNCTION*>* bin_funcs;
public:
    PartHashAlgorithm(CONFIG* config) : Algorithm(config) {}
    void process_binary(std::vector<FUNCTION*>* bin_funcs) override;
    void process_lib(std::string lib_path, std::vector<FUNCTION*>* lib_funcs) override;
};

#endif //CERBERUS_PART_HASH_ALGORITHM_H
