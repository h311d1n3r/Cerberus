#ifndef CERBERUS_LIB_REGEX_H
#define CERBERUS_LIB_REGEX_H

#include <vector>
#include <string>
#include <map>
#include <langs/lang_types.h>
#include <binaries/bin_types.h>
#include <functional>

std::vector<std::string> rust_lib_regex = {
    "/.cargo/(.+?)\\.rs",
    "/cargo/(.+?)\\.rs"
};

std::vector<std::string> go_lib_regex = {
    "go(.*?)/pkg/mod/(.+?)\\.(s|go)",
    "go(.*?)/src/(.+?)\\.(s|go)"
};

using LibExtractCallback = std::function<LIBRARY*(std::string)>;

LIBRARY* rust_extract_callback(std::string match);
LIBRARY* go_extract_callback(std::string match);

extern std::map<LANG, LibExtractCallback> lib_extract_callbacks;

#endif //CERBERUS_LIB_REGEX_H
