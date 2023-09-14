#include <binaries/bin_handler.h>
#include <binaries/lib/install/lib_installer.h>
#include <binaries/lib/install/rust_lib_installer.h>
#include <binaries/lib/install/go_lib_installer.h>
#include <langs/lib_regex.h>
#include <utils/search.h>
#include <utils/convert.h>

using namespace std;

BIN_ARCH BinaryHandler::extract_architecture() {
    this->arch = lief_extractor->extract_arch();
    return this->arch;
}

size_t BinaryHandler::libs_extraction() {
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
        vector<string> matches = search_regex(data, bin_file_sz, reg);
        for(string match : matches) {
            unique_ptr<LIBRARY> lib = lib_extract_callbacks[lang](match);
            if(lib) {
                bool exists = false;
                for(unique_ptr<LIBRARY>& lib2 : this->libs) {
                    if(lib->name == lib2->name && lib->version == lib2->version) {
                        exists = true;
                        break;
                    }
                }
                if(!exists) this->libs.push_back(move(lib));
            }
        }
    }
    bin_file.close();
    return this->libs.size();
}

size_t BinaryHandler::libs_installation() {
    unique_ptr<LibInstaller> installer;
    switch(lang) {
        case RUST:
            installer = make_unique<RustLibInstaller>(this->work_dir, this->arch);
            break;
        case GO:
            installer = make_unique<GoLibInstaller>(this->work_dir, this->arch);
        default:
            return 0;
    }
    size_t success_ctr = 0;
    for(std::unique_ptr<LIBRARY>& lib : this->libs) {
        if(installer->install_lib(*lib.get())) success_ctr++;
    }
    return success_ctr;
}

size_t BinaryHandler::get_matches_sz() {
    size_t matches = 0;
    for(unique_ptr<FUNCTION>& func : this->functions) if(func->name.size()) matches++;
    return matches;
}

void BinaryHandler::demangle_functions() {
    for(unique_ptr<FUNCTION>& func : this->functions) {
        if(func->name.size()) func->name = demangle_function_name(func->name);
    }
}