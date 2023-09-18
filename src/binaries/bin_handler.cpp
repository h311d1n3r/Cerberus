#include <binaries/bin_handler.h>
#include <binaries/lib/install/lib_installer.h>
#include <binaries/lib/install/rust_lib_installer.h>
#include <binaries/lib/install/go_lib_installer.h>
#include <langs/lib_regex.h>
#include <utils/search.h>
#include <utils/convert.h>
#include <utils/logger.h>

using namespace std;

BIN_ARCH BinaryHandler::extract_architecture() {
    this->arch = lief_extractor->extract_arch();
    return this->arch;
}

void BinaryHandler::extract_image_base() {
    this->image_base = lief_extractor->extract_image_base();
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
        vector<string> matches = search_regex(data, bin_file_sz, reg, 256);
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
    sort(this->libs.begin(), this->libs.end(), [](const unique_ptr<LIBRARY>& a, const unique_ptr<LIBRARY>& b) {
        return a->name < b->name;
    });
    bin_file.close();
    return this->libs.size();
}

size_t BinaryHandler::libs_installation() {
    unique_ptr<LibInstaller> installer;
    switch(lang) {
        case RUST:
            installer = make_unique<RustLibInstaller>(this->work_dir, this->arch, this->type);
            break;
        case GO:
            installer = make_unique<GoLibInstaller>(this->work_dir, this->arch, this->type);
            break;
        default:
            return 0;
    }
    if(!installer->pre_install_hook(this->libs)) return 0;
    size_t success_ctr = 0;
    for(std::unique_ptr<LIBRARY>& lib : this->libs) {
        fcout << "$(info)Installing $(bright_magenta:b)" << lib->name << "$$(red):$$(magenta:b)" << lib->version << "$..." << endl;
        if(installer->install_lib(*lib.get())) {
            success_ctr++;
            fcout << "$(success)Success !" << endl;
        } else fcout << "$(error)Failure..." << endl;
    }
    if(!installer->post_install_hook()) return 0;
    return success_ctr;
}

size_t BinaryHandler::get_matches_sz() {
    for(unique_ptr<FUNCTION>& func : this->functions) if(func->name.size()) matches_sz++;
    return matches_sz;
}

void BinaryHandler::demangle_functions() {
    for(unique_ptr<FUNCTION>& func : this->functions) {
        if(func->name.size()) func->name = demangle_function_name(func->name);
    }
}