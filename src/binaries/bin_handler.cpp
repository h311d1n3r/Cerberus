#include <binaries/bin_handler.h>
#include <binaries/lib/install/lib_installer.h>
#include <binaries/lib/install/rust_lib_installer.h>
#include <binaries/lib/install/go_lib_installer.h>
#include <langs/lib_regex.h>
#include <utils/search.h>
#include <fstream>

using namespace std;

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
            LIBRARY* lib = lib_extract_callbacks[lang](match);
            if(lib) {
                bool exists = false;
                for(LIBRARY* lib2 : this->libs) {
                    if(lib->name == lib2->name && lib->version == lib2->version) {
                        exists = true;
                        break;
                    }
                }
                if(!exists) this->libs.push_back(lib);
            }
        }
    }
    bin_file.close();
    return this->libs.size();
}

size_t BinaryHandler::libs_installation() {
    LibInstaller* installer;
    switch(lang) {
        case RUST:
            installer = new RustLibInstaller(this->work_dir);
            break;
        case GO:
            installer = new GoLibInstaller(this->work_dir);
        default:
            return 0;
    }
    size_t success_ctr = 0;
    for(LIBRARY* lib : this->libs) {
        if(installer->install_lib(lib)) success_ctr++;
    }
    return success_ctr;
}

void BinaryHandler::function_hashing(FUNCTION* func) {

}