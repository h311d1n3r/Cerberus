#include <binaries/lib/install/go_lib_installer.h>
#include <command/command_executor.h>
#include <utils/logger.h>
#include <algorithm>
#include <fstream>

using namespace std;

bool GoLibInstaller::pre_install_hook(vector<std::unique_ptr<LIBRARY>>& libs) {
    filesystem::create_directory(this->work_dir+"/standalone");
    auto go_lib_it = find_if(libs.begin(), libs.end(), [](const unique_ptr<LIBRARY>& item) {
        return item->name == "go";
    });
    if(go_lib_it == libs.end()) return false;
    LIBRARY* go_lib = go_lib_it->get();
    libs.erase(go_lib_it);
    if(!this->install_go_version(go_lib->version)) return false;
    CommandExecutor executor(this->work_dir+"/standalone");
    COMMAND_RESULT res;
    executor.execute_command("../go mod init build_mod", &res);
    if(res.code || res.response.find("creating new go.mod") == string::npos) return false;
    ofstream build_mod_file(this->work_dir+"/standalone/build_mod.go");
    build_mod_file << "package main" << endl << endl;
    build_mod_file << "import (" << endl;
    for(std::unique_ptr<LIBRARY>& lib : libs) {
        string lib_name = lib->name;
        if(!lib_name.find("std::")) lib_name = lib_name.substr(string("std::").length());
        build_mod_file << "\t_ \"" << lib_name << "\"" << endl;
    }
    build_mod_file << ")" << endl << endl;
    build_mod_file << "func main() {}";
    build_mod_file.close();
    return true;
}

bool GoLibInstaller::install_go_version(std::string version) {
    fcout << "$(info)Installing $(bright_magenta:b)go$$(red):$$(magenta:b)" << version << "$..." << endl;
    CommandExecutor executor(work_dir);
    COMMAND_RESULT res;
    executor.execute_command("goliath "+version, &res);
    bool success = !res.code;
    this->goliath_output_dir = this->work_dir+"/go-"+version;
    if(success) {
        filesystem::rename(goliath_output_dir+"/go/bin/go", this->work_dir+"/go");
        fcout << "$(success)Success !" << endl;
    } else fcout << "$(error)Failure..." << endl;
    return success;
}

bool GoLibInstaller::install_lib(LIBRARY lib) {
    CommandExecutor executor(this->work_dir+"/standalone");
    COMMAND_RESULT res;
    string lib_name = lib.name;
    if(!lib_name.find("std::")) lib_name = lib_name.substr(string("std::").length());
    if(lib.version != "unk") lib_name += "@v"+lib.version;
    executor.execute_command("../go get "+lib_name, &res);
    return !res.code;
}

bool GoLibInstaller::post_install_hook() {
    fcout << "$(info)Building standalone library..." << endl;
    CommandExecutor executor(this->work_dir+"/standalone");
    COMMAND_RESULT res;
    executor.execute_command("../go build -o ./standalone.so build_mod.go", &res);
    bool success = !res.code;
    if(success) {
        filesystem::rename(this->work_dir+"/standalone/standalone.so", this->work_dir+"/standalone.so");
        fcout << "$(success)Success !" << endl;
    }
    else fcout << "$(error)Failure..." << endl;
    filesystem::remove(this->work_dir+"/go");
    filesystem::remove_all(this->goliath_output_dir);
    filesystem::remove_all(this->work_dir+"/standalone");
    return success;
}