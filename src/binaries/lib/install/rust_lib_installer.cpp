#include <binaries/lib/install/rust_lib_installer.h>
#include <utils/file_operations.h>
#include <utils/convert.h>
#include <filesystem>
#include <fstream>
#include <vector>
#include <string>

namespace fs = std::filesystem;
using namespace std;

bool RustLibInstaller::install_lib(LIBRARY *lib) {
    string output_dir_name = this->work_dir+"/"+lib->name+"-"+lib->version;
    string zip_file_name = output_dir_name+".crate";
    string tar_file_name = output_dir_name+".tar";
    if(!this->downloader->download_file("https://crates.io/api/v1/crates/"+lib->name+"/"+lib->version+"/download", zip_file_name)) return false;
    if(!decompress_gzip_file(zip_file_name, tar_file_name)) return false;
    fs::remove(zip_file_name);
    if(!decompress_tar_file(tar_file_name, output_dir_name)) return false;
    fs::remove(tar_file_name);
    ifstream cargo_toml_input(output_dir_name+"/Cargo.toml");
    if(!cargo_toml_input.is_open()) return false;
    vector<string> cargo_toml_lines;
    string line;
    bool found_lib = false;
    while(getline(cargo_toml_input, line)) {
        strip(line);
        if(line.find("[lib]") != string::npos) {
            found_lib = true;
            cargo_toml_lines.push_back("[lib]");
            cargo_toml_lines.push_back("crate-type = [\"dylib\"]");
        } else if(line.find("crate-type ") && line.find("crate-type=")) cargo_toml_lines.push_back(line);
    }
    cargo_toml_input.close();
    if(!found_lib) {
        cargo_toml_lines.push_back("[lib]");
        cargo_toml_lines.push_back("crate-type = [\"dylib\"]");
    }
    ofstream cargo_toml_output(output_dir_name+"/Cargo.toml");
    for(string line : cargo_toml_lines) cargo_toml_output.write((line+string("\n")).c_str(), line.size()+1);
    cargo_toml_output.close();
    return true;
}