#include <langs/lib_regex.h>
#include <utils/convert.h>
#include <iostream>

using namespace std;

std::vector<std::string> rust_lib_regex = {
    "/.cargo/(.+?)\\.rs",
    "/cargo/(.+?)\\.rs"
};

std::vector<std::string> go_lib_regex = {
    "go(.*?)/pkg/mod/(.+?)\\.(s|go)",
    "go(.*?)/src/(.+?)\\.(s|go)"
};

unique_ptr<LIBRARY> rust_extract_callback(string match) {
    size_t null_term_index;
    if((null_term_index = match.find('\x00')) != string::npos) {
        match = match.substr(0, null_term_index);
    }
    vector<string> match_parts = split_string(match, '/');
    if(match_parts.size() < 6) return nullptr;
    string lib_and_version = match_parts.at(5);
    size_t delim_index;
    if((delim_index = lib_and_version.rfind('-')) == string::npos) return nullptr;
    unique_ptr<LIBRARY> lib = make_unique<LIBRARY>();
    lib->name = lib_and_version.substr(0, delim_index);
    lib->version = lib_and_version.substr(delim_index+1);
    return lib;
}

unique_ptr<LIBRARY> go_extract_callback(string match) {
    size_t null_term_index;
    if((null_term_index = match.find('\x00')) != string::npos) {
        match = match.substr(0, null_term_index);
    }
    vector<string> match_parts = split_string(match, '/');

    return nullptr;
}

map<LANG, LibExtractCallback> lib_extract_callbacks = {
    {LANG::RUST, rust_extract_callback},
    {LANG::GO, go_extract_callback}
};