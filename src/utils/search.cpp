#include <utils/search.h>

using namespace std;

vector<string> search_regex(char* data, size_t data_sz, string pattern) {
    vector<string> matches;
    regex reg(pattern);
    string data_str(data, data_sz);
    sregex_iterator it(data_str.begin(), data_str.end(), reg);
    sregex_iterator end;
    while(it != end) {
        smatch match = *it;
        matches.push_back(match.str());
        ++it;
    }
    return matches;
}