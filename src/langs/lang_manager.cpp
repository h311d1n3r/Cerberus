#include <langs/lang_manager.h>

using namespace std;

map<LANG, std::string> name_from_lang = {
    {LANG::Rust, "Rust"},
    {LANG::Go, "Go"}
};

vector<std::pair<std::string, LANG>> LANG_PATTERNS = {
        std::pair("/rustc-", LANG::Rust),
        std::pair("/.cargo/", LANG::Rust),
        std::pair("/go-", LANG::Go),
        std::pair("runtime.go", LANG::Go)
};