#include <user/dependencies/dependency_manager.h>
#include <utils/logger.h>

using namespace std;

map<PACKAGE_MANAGER, string> install_commands = {
    {PACKAGE_MANAGER::APT, "apt install %s"},
    {PACKAGE_MANAGER::APT_GET, "apt-get install %s"},
    {PACKAGE_MANAGER::DNF, "dnf install %s"},
    {PACKAGE_MANAGER::YUM, "yum install %s"},
    {PACKAGE_MANAGER::ZYPPER, "zypper install %s"},
    {PACKAGE_MANAGER::PACMAN, "pacman -S %s"},
    {PACKAGE_MANAGER::PORTAGE, "emerge %s"},
    {PACKAGE_MANAGER::SLACKPKG, "slackpkg install %s"},
    {PACKAGE_MANAGER::SWARET, "swaret --install %s"},
    {PACKAGE_MANAGER::XBPS, "xbps-install -S %s"},
    {PACKAGE_MANAGER::APK, "apk add %s"},
    {PACKAGE_MANAGER::NIX, "nix-env -iA nixpkgs.%s"},
    {PACKAGE_MANAGER::PETGET, "petget %s.pet"}
};

void DependencyManager::set_password(string password) {
    this->password = password;
}

bool DependencyManager::is_package_installed(PACKAGE* package) {
    return is_binary_on_path(package->binary);
}

bool DependencyManager::install_package(OS_PACKAGE* package) {
    fcout << "$(info)Installing $(info:b)" << package->name << "$..." << endl;
    string command = install_commands[config->package_manager];
    char formatted_cmd_buf[command.length() - 2 + package->name.length() + 1];
    snprintf(formatted_cmd_buf, sizeof(formatted_cmd_buf), command.c_str(), package->name.c_str());
    string formatted_cmd(formatted_cmd_buf);
    if(config->is_root) {
        COMMAND_RESULT res;
        executor.execute_command(formatted_cmd, &res);
        return res.code == 0;
    }
    else if(config->has_sudo) {
        COMMAND_RESULT res;
        executor.execute_command(string("echo ")+password+string(" | sudo -S ")+formatted_cmd, &res);
        return res.code == 0;
    }
    return false;
}

bool DependencyManager::install_package(GIT_PACKAGE* package) {
    fcout << "$(info)Installing $(info:b)" << package->repo_name << "$..." << endl;
    COMMAND_RESULT res;
    executor.execute_command(string("git clone ")+package->url, &res);
    if(res.code != 0) return false;
    string build_cmd = "mkdir build; cd build; cmake ..; make; make install";
    if(package->custom_command.size()) build_cmd = package->custom_command;
    if(!config->is_root && package->needs_root) build_cmd = string("echo ")+password+string(" | sudo -S sh -c \"unset SUDO_USER ; ") + build_cmd + string("\"");
    executor.execute_command(string("cd ")+package->repo_name+string("; ")+build_cmd, &res);
    if(package->remove_dir) filesystem::remove_all(work_dir+string("/")+package->repo_name);
    return res.code == package->success_code;
}

bool DependencyManager::install_package(PIP3_PACKAGE *package) {
    fcout << "$(info)Installing $(info:b)" << package->package_name << "$..." << endl;
    COMMAND_RESULT res;
    executor.execute_command(string("pip3 install ")+package->package_name, &res);
    return !res.code;
}

bool DependencyManager::install_package(CARGO_PACKAGE *package) {
    fcout << "$(info)Installing $(info:b)" << package->package_name << "$..." << endl;
    COMMAND_RESULT res;
    executor.execute_command(string("cargo install ")+package->package_name, &res);
    return !res.code;
}

bool DependencyManager::install_package(CUSTOM_PACKAGE *package) {
    fcout << "$(info)Installing $(info:b)" << package->package_name << "$..." << endl;
    COMMAND_RESULT res;
    executor.execute_command(string("sh -c \"") + package->command + string("\""), &res);
    return res.code == package->success_code;
}