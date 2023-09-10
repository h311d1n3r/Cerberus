#include <utils/convert.h>
#include <sstream>

using namespace std;

vector<string> split_string(const string& input, char delimiter) {
    vector<std::string> tokens;
    istringstream token_stream(input);
    string token;
    while (getline(token_stream, token, delimiter)) {
        tokens.push_back(token);
    }
    return tokens;
}

string strip(const string& str) {
    size_t first = str.find_first_not_of(" \t\n");
    if (string::npos == first) {
        return "";
    }
    size_t last = str.find_last_not_of(" \t\n");
    return str.substr(first, (last - first + 1));
}