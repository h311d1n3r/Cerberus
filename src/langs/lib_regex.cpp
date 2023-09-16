#include <langs/lib_regex.h>
#include <utils/convert.h>

using namespace std;

std::vector<std::string> rust_lib_regex = {
    "/.cargo/(.+?)\\.rs",
    "/cargo/(.+?)\\.rs"
};

std::vector<std::string> go_lib_regex = {
    "go(.*?)/pkg/mod/(.+?)\\.(s|go)",
    "go(.*?)/src/(.+?)\\.(s|go)",
    "go\\d+\\.\\d+\\.\\d+\\x00",
    "go\\d+\\.\\d+\\x00"
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
    if(match.find("/") == string::npos && match.length() > 2 && isdigit(match.at(2))) {
        unique_ptr<LIBRARY> lib = make_unique<LIBRARY>();
        lib->name = "go";
        lib->version = match.substr(2);
        return lib;
    }
    size_t pkg_index, src_index;
    if((pkg_index = match.find("/pkg/mod/")) != string::npos) {
        size_t version_index = match.find("@");
        if(match.find("golang.org") == string::npos && version_index != string::npos) {
            unique_ptr<LIBRARY> lib = make_unique<LIBRARY>();
            size_t name_start_index = pkg_index+string("/pkg/mod/").length();
            lib->name = match.substr(name_start_index, version_index-name_start_index);
            lib->version = match.substr(version_index+2);
            lib->version = lib->version.substr(0, lib->version.find('/'));
            return lib;
        }
    } else if((src_index = match.find("/src/")) != string::npos) {
        const string forbidden_list[] = {"internal","runtime","github.com","golang.org"};
        for(string forbidden : forbidden_list) if(match.find(forbidden) != string::npos) return nullptr;
        match = match.substr(src_index+string("/src/").length());
        match = match.substr(0, match.find_last_of('/'));
        unique_ptr<LIBRARY> lib = make_unique<LIBRARY>();
        lib->name = "std::" + match;
        lib->version = "unk";
        return lib;
    }
    return nullptr;
}

map<LANG, LibExtractCallback> lib_extract_callbacks = {
    {LANG::RUST, rust_extract_callback},
    {LANG::GO, go_extract_callback}
};