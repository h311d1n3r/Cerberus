#include <binaries/lib/install/rust_lib_installer.h>
#include <utils/file_operations.h>
#include <utils/convert.h>
#include <filesystem>
#include <fstream>
#include <vector>
#include <string>
#include <map>
#include <command/command_executor.h>
#include <utils/logger.h>
#include <user/user_prompt.h>

namespace fs = std::filesystem;
using namespace std;

const string RBF_PREFIX = "$(bright_blue:b)[$(blue:b)RBF$]$ ";

void newer_edition_patch(string crate_path) {
    if(fs::exists(crate_path+"/Cargo.toml")) {
        ifstream cargo_input_file(crate_path + "/Cargo.toml");
        string line;
        vector<string> lines;
        while(getline(cargo_input_file, line)) {
            line = strip(line);
            if(line.find("[package]") != string::npos) {
                lines.push_back("[package]");
                lines.push_back("edition = \"2021\"");
            } else if(line.find("edition ") == string::npos && line.find("edition=") == string::npos) {
                lines.push_back(line);
            }
        }
        cargo_input_file.close();
        ofstream cargo_output_file(crate_path+"/Cargo.toml");
        for(string l : lines) {
            cargo_output_file.write((l + string("\n")).c_str(), l.size() + 1);
        }
        cargo_output_file.close();
    }
}

PATCH NEWER_EDITION_PATCH{
    "EDITION 2021",
    newer_edition_patch
};

void std_redefinition_patch(string crate_path) {
    if(fs::exists(crate_path+"/src/lib.rs")) {
        ifstream lib_input_file(crate_path+"/src/lib.rs");
        string line;
        vector<string> lines;
        while(getline(lib_input_file, line)) {
            if(line.find("no_std") == string::npos && line.find("as std;") == string::npos) lines.push_back(line);
        }
        lib_input_file.close();
        ofstream lib_output_file(crate_path+"/src/lib.rs");
        for(string l : lines) {
            lib_output_file.write((l + string("\n")).c_str(), l.size() + 1);
        }
        lib_output_file.close();
    }
}

PATCH STD_REDEFINITION_PATCH {
    "STD REDEFINITION",
    std_redefinition_patch
};

void add_workspace_patch(string crate_path) {
    if(fs::exists(crate_path+"/Cargo.toml")) {
        ifstream cargo_input_file(crate_path+"/Cargo.toml");
        vector<string> lines;
        string line;
        while(getline(cargo_input_file, line)) {
            lines.push_back(line);
        }
        lines.push_back("[workspace]");
        cargo_input_file.close();
        ofstream cargo_output_file(crate_path+"/Cargo.toml");
        for(string l : lines) {
            cargo_output_file.write((l + string("\n")).c_str(), l.size() + 1);
        }
        cargo_output_file.close();
    }
}

PATCH ADD_WORKSPACE_PATCH {
    "ADD WORKSPACE",
    add_workspace_patch
};

map<string, PATCH> patches = {
    {"maybe a missing crate `core`?", NEWER_EDITION_PATCH},
    {"the name `std` is defined multiple times", STD_REDEFINITION_PATCH},
    {"language item required, but not found: `eh_personality`", STD_REDEFINITION_PATCH},
    {"current package believes it's in a workspace when it's not", ADD_WORKSPACE_PATCH}
};

bool RustBuildFixer::process_error(string command, string error) {
    for(pair<string, PATCH> patch_pair : patches) {
        if(error.find(patch_pair.first) != string::npos) {
            PATCH patch = patch_pair.second;
            if(last_patch && patch.patch_func == last_patch->patch_func) return false;
            last_patch = &patch;
            fcout << "$(info)" << RBF_PREFIX << "Applying patch $(red:b)" << patch.name << "$..." << endl;
            patch.patch_func(crate_path);
            COMMAND_RESULT res;
            executor.execute_command(command, &res);
            if(!res.code) return true;
            return process_error(command, res.response);
        }
    }
    return false;
}

void RustLibInstaller::check_and_install_arch(string arch_name) {
    CommandExecutor executor("./");
    COMMAND_RESULT res;
    executor.execute_command("rustup target list", &res);
    if(res.code) return;
    vector<string> lines = split_string(res.response, '\n');
    for(string& line : lines) {
        if(line.find(arch_name) != string::npos) {
            if(line.find("installed") == string::npos) {
                fcout << "$(info)You need to install the toolchain for $(info:b)" << arch_name << "$ architecture." << endl;
                bool agree_install = ask_yes_no("Proceed to installation ?", true);
                if(agree_install) {
                    COMMAND_RESULT res;
                    string install_cmd = "rustup target install " + arch_name;
                    executor.execute_command(install_cmd, &res);
                    if(res.code) fcout << "$(error)An error occurred during installation... Run $(error:b)" << install_cmd << "$ for more information." << endl;
                    else fcout << "$(success)Done !" << endl;
                    return;
                } else return;
            } else {
                fcout << "$(info)Requested architecture is already installed." << endl;
                return;
            }
        }
    }
}

RustLibInstaller::RustLibInstaller(string work_dir, BIN_ARCH arch) : LibInstaller(work_dir, arch), downloader() {
    if(arch == BIN_ARCH::X86) check_and_install_arch("i686-unknown-linux-gnu");
}

bool RustLibInstaller::install_lib(LIBRARY lib) {
    fcout << "$(info)Installing $(bright_magenta:b)" << lib.name << "$$(red):$$(magenta:b)" << lib.version << "$..." << endl;
    string output_dir_name = this->work_dir+"/"+lib.name+"-"+lib.version;
    string zip_file_name = output_dir_name+".crate";
    string tar_file_name = output_dir_name+".tar";
    if(!this->downloader.download_file("https://crates.io/api/v1/crates/"+lib.name+"/"+lib.version+"/download", zip_file_name)) {
        fcout << "$(error)Failure..." << endl;
        return false;
    }
    if(!decompress_gzip_file(zip_file_name, tar_file_name)) {
        fcout << "$(error)Failure..." << endl;
        return false;
    }
    fs::remove(zip_file_name);
    if(!decompress_tar_file(tar_file_name, output_dir_name)) {
        fcout << "$(error)Failure..." << endl;
        return false;
    }
    fs::remove(tar_file_name);
    ifstream cargo_toml_input(output_dir_name+"/Cargo.toml");
    if(!cargo_toml_input.is_open()) {
        fcout << "$(error)Failure..." << endl;
        return false;
    }
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
    if(!fs::exists(output_dir_name+"/Cargo.toml")) {
        fcout << "$(error)Failure..." << endl;
        return false;
    }
    ofstream cargo_toml_output(output_dir_name+"/Cargo.toml");
    for(string line : cargo_toml_lines) cargo_toml_output.write((line+string("\n")).c_str(), line.size()+1);
    cargo_toml_output.close();
    CommandExecutor executor(output_dir_name);
    COMMAND_RESULT res;
    string command = "cargo build --release";
    if(arch == BIN_ARCH::X86) command += " --target=i686-unknown-linux-gnu";
    executor.execute_command(command, &res);
    bool success = res.code == 0;
    if(!success) {
        fcout << "$(warning)An error occurred during build, delegating to $(warning:b)RBF$ (Rust Build Fixer)..." << endl;
        success = RustBuildFixer(output_dir_name).process_error(command, res.response);
    }
    if(success) fcout << "$(success)Success !" << endl;
    else {
        fcout << "$(error)Failure..." << endl;
        return false;
    }
    string release_dir = output_dir_name+string("/target/release");
    if(fs::exists(release_dir)) {
        for (const auto& entry : fs::directory_iterator(release_dir)) {
            if (fs::is_regular_file(entry)) {
                fs::path file_path = entry.path();
                string file_name = file_path.filename();
                if(ends_with(file_name, ".so")) {
                    fs::rename(file_path, this->work_dir+"/"+file_name);
                }
            }
        }
    }
    fs::remove_all(output_dir_name);
    return true;
}