#include <binaries/bin_handler.h>
#include <langs/lib_regex.h>
#include <utils/search.h>
#include <fstream>

using namespace std;

size_t BinaryHandler::libs_extraction(LANG lang) {
    vector<string> lib_regex;
    switch(lang) {
        case LANG::RUST:
            lib_regex = rust_lib_regex;
            break;
        case LANG::GO:
            lib_regex = go_lib_regex;
            break;
        default:
            return 0;
    }
    ifstream bin_file(this->bin_path, ios::binary);
    bin_file.seekg(0, ios::end);
    size_t bin_file_sz = bin_file.tellg();
    bin_file.seekg(0);
    char data[bin_file_sz];
    bin_file.read(data, bin_file_sz);
    for(string reg : lib_regex) {
        vector<smatch> matches = search_regex(data, bin_file_sz, reg);
        for(smatch match : matches) {
            LIBRARY* lib = lib_extract_callbacks[lang](match.str());
            if(lib) {
                this->libs.push_back(lib);
            }
        }
    }
    bin_file.close();
    return this->libs.size();
}

void BinaryHandler::function_hashing(FUNCTION* func) {

}