#ifndef CERBERUS_LIB_REGEX_H
#define CERBERUS_LIB_REGEX_H

#include <vector>
#include <string>
#include <map>
#include <langs/lang_types.h>
#include <binaries/bin_types.h>
#include <functional>
#include <memory>

extern std::vector<std::string> rust_lib_regex;

extern std::vector<std::string> go_lib_regex;

using LibExtractCallback = std::function<std::unique_ptr<LIBRARY>(std::string)>;

std::unique_ptr<LIBRARY> rust_extract_callback(std::string match);
std::unique_ptr<LIBRARY> go_extract_callback(std::string match);

extern std::map<LANG, LibExtractCallback> lib_extract_callbacks;

#endif //CERBERUS_LIB_REGEX_H
