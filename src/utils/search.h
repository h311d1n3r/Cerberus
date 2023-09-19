#ifndef CERBERUS_SEARCH_H
#define CERBERUS_SEARCH_H

#include <regex>
#include <vector>

std::vector<std::string> search_regex(char* data, size_t data_sz, std::string pattern, size_t match_max_sz);

#endif //CERBERUS_SEARCH_H
