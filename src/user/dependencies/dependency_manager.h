#ifndef CERBERUS_DEPENDENCY_MANAGER_H
#define CERBERUS_DEPENDENCY_MANAGER_H

#include <string>
#include <map>
#include <user/local_config.h>
#include <command/command_executor.h>

struct PACKAGE {
    uint8_t package_type;
    std::string binary;
};

struct OS_PACKAGE : PACKAGE {
    std::string name;
    OS_PACKAGE(std::string name, std::string binary) {
        PACKAGE::package_type = 0;
        PACKAGE::binary = binary;
        OS_PACKAGE::name = name;
    }
};

struct PIP3_PACKAGE : PACKAGE {
    std::string package_name;
    PIP3_PACKAGE(std::string binary, std::string package_name) {
        PACKAGE::package_type = 1;
        PACKAGE::binary = binary;
        PIP3_PACKAGE::package_name = package_name;
    }
};

struct GIT_PACKAGE : PACKAGE {
    std::string repo_name;
    std::string url;
    std::string custom_command = "";
    std::int32_t success_code = 0;
    bool remove_dir = true;
    bool needs_root = true;
    GIT_PACKAGE(std::string repo_name, std::string binary, std::string url) {
        package_type = 2;
        PACKAGE::binary = binary;
        GIT_PACKAGE::repo_name = repo_name;
        GIT_PACKAGE::url = url;
    }
    GIT_PACKAGE(std::string repo_name, std::string binary, std::string url, int32_t success_code) : GIT_PACKAGE(repo_name, binary, url) {
        GIT_PACKAGE::success_code = success_code;
    }
    GIT_PACKAGE(std::string repo_name, std::string binary, std::string url, int32_t success_code, std::string custom_command) : GIT_PACKAGE(repo_name, binary, url, success_code) {
        GIT_PACKAGE::custom_command = custom_command;
    }
    GIT_PACKAGE(std::string repo_name, std::string binary, std::string url, int32_t success_code, std::string custom_command, bool remove_dir) : GIT_PACKAGE(repo_name, binary, url, success_code, custom_command) {
        GIT_PACKAGE::remove_dir = remove_dir;
    }
    GIT_PACKAGE(std::string repo_name, std::string binary, std::string url, int32_t success_code, std::string custom_command, bool remove_dir, bool needs_root) : GIT_PACKAGE(repo_name, binary, url, success_code, custom_command, remove_dir) {
        GIT_PACKAGE::needs_root = needs_root;
    }
};

struct CARGO_PACKAGE : PACKAGE {
    std::string package_name;
    CARGO_PACKAGE(std::string binary, std::string package_name) {
        PACKAGE::package_type = 3;
        PACKAGE::binary = binary;
        CARGO_PACKAGE::package_name = package_name;
    }
};

struct GO_PACKAGE : PACKAGE {
    std::string package_name;
    std::string url;
    GO_PACKAGE(std::string binary, std::string package_name, std::string url) {
        PACKAGE::package_type = 4;
        PACKAGE::binary = binary;
        GO_PACKAGE::package_name = package_name;
        GO_PACKAGE::url = url;
    }
};

struct CUSTOM_PACKAGE : PACKAGE {
    std::string package_name;
    std::string command;
    int32_t success_code = 0;
    CUSTOM_PACKAGE(std::string package_name, std::string binary, std::string command) {
        PACKAGE::package_type = 5;
        PACKAGE::binary = binary;
        CUSTOM_PACKAGE::package_name = package_name;
        CUSTOM_PACKAGE::command = command;
    }
    CUSTOM_PACKAGE(std::string package_name, std::string binary, std::string command, int32_t success_code) : CUSTOM_PACKAGE(package_name, binary, command) {
        CUSTOM_PACKAGE::success_code = success_code;
    }
};

extern std::map<PACKAGE_MANAGER, std::string> install_commands;

class DependencyManager {
private:
    LOCAL_CONFIG* config;
    std::string work_dir;
    CommandExecutor executor;
    std::string password;
public:
    DependencyManager(LOCAL_CONFIG* config, std::string work_dir) : config(config), work_dir(work_dir), executor(work_dir) {};
    void set_password(std::string password);
    bool is_package_installed(PACKAGE* package);
    bool install_package(OS_PACKAGE* package);
    bool install_package(GIT_PACKAGE* package);
    bool install_package(PIP3_PACKAGE* package);
    bool install_package(CARGO_PACKAGE* package);
    bool install_package(GO_PACKAGE* package);
    bool install_package(CUSTOM_PACKAGE* package);
};

#endif //CERBERUS_DEPENDENCY_MANAGER_H
