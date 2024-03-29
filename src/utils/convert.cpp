#include <utils/convert.h>
#include <sstream>
#include <command/command_executor.h>

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

vector<std::string> filter_empty_strings(const vector<std::string>& tab) {
    vector<std::string> result;
    for (const std::string& s : tab) {
        if (!s.empty()) {
            result.push_back(s);
        }
    }
    return result;
}

string strip(const string& str) {
    size_t first = str.find_first_not_of(" \t\n");
    if (string::npos == first) {
        return "";
    }
    size_t last = str.find_last_not_of(" \t\n");
    return str.substr(first, (last - first + 1));
}

bool ends_with(string const &value, string const &ending) {
    if (ending.size() > value.size()) return false;
    return equal(ending.rbegin(), ending.rend(), value.rbegin());
}

void replace_all(string& str, const string& from, const string& to) {
    if(from.empty()) return;
    size_t start_pos = 0;
    while((start_pos = str.find(from, start_pos)) != std::string::npos) {
        str.replace(start_pos, from.length(), to);
        start_pos += to.length();
    }
}

string demangle_function_name(string& mangled_name) {
    replace_all(mangled_name, string("$"), string("\\$"));
    CommandExecutor executor("./");
    COMMAND_RESULT res;
    executor.execute_command(string("c++filt \"")+mangled_name+string("\""), &res);
    if(!res.code) return res.response.substr(0, res.response.length()-1);
    return mangled_name;
}