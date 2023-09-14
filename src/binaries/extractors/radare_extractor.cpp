#include <binaries/extractors/radare_extractor.h>
#include <utils/convert.h>
#include <iostream>

using namespace std;

BIN_ARCH RadareExtractor::extract_arch() {

}

vector<unique_ptr<FUNCTION>> RadareExtractor::extract_functions(BIN_ARCH arch) {
    vector<unique_ptr<FUNCTION>> funcs;
    COMMAND_RESULT res;
    executor.execute_command(string("radare2 -q -c aaa -c afl \"") + bin_path + string("\""), &res);
    if (!res.code) {
        vector<string> lines = split_string(res.response, '\n');
        for(string& line : lines) {
            if(line.length() < 2 || line.substr(0, 2) != "0x") continue;
            vector<string> vals = split_string(line, ' ');
            vals = filter_empty_strings(vals);
            unique_ptr<FUNCTION> func = make_unique<FUNCTION>();
            func->start = stoull(vals[0].substr(2), nullptr, 16);
            func->end = func->start + stoull(vals[2]) - 1;
            func->name = vals[3];
            funcs.push_back(move(func));
        }
    }
    return funcs;
}

vector<unique_ptr<SECTION>> RadareExtractor::extract_sections() {

}