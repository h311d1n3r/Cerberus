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