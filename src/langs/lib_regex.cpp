#include <langs/lib_regex.h>
#include <utils/convert.h>

using namespace std;

LIBRARY* rust_extract_callback(string match) {
    size_t null_term_index;
    if((null_term_index = match.find('\x00')) != string::npos) {
        match = match.substr(0, null_term_index);
    }
    vector<string> match_parts = split_string(match, '/');
}

LIBRARY* go_extract_callback(string match) {

}

map<LANG, LibExtractCallback> lib_extract_callbacks = {
    {LANG::RUST, rust_extract_callback},
    {LANG::GO, go_extract_callback}
};