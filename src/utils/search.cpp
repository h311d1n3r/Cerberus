#include <utils/search.h>

using namespace std;

vector<string> search_regex(char* data, size_t data_sz, string pattern, size_t match_max_sz) {
    vector<string> matches;
    regex reg(pattern);
    size_t data_i = 0;
    while(data_i < data_sz) {
        string data_str(data+data_i, 1024+match_max_sz);
        sregex_iterator it(data_str.begin(), data_str.end(), reg);
        sregex_iterator end;
        while (it != end) {
            smatch match = *it;
            matches.push_back(match.str());
            ++it;
        }
        data_i += 1024;
    }
    return matches;
}