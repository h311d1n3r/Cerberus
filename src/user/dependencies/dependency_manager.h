#ifndef CERBERUS_DEPENDENCY_MANAGER_H
#define CERBERUS_DEPENDENCY_MANAGER_H

#include <string>
#include <map>
#include <user/local_config.h>
#include <command/command_executor.h>

struct PACKAGE {
    bool os;
    std::string binary;
};

struct OS_PACKAGE : PACKAGE {
    std::string name;
    OS_PACKAGE(std::string name, std::string binary) {
        PACKAGE::os = true;
        PACKAGE::binary = binary;
        OS_PACKAGE::name = name;
    }
};

struct GIT_PACKAGE : PACKAGE {
    std::string repo_name;
    std::string url;
    std::string custom_command = "";
    std::int32_t success_code = 0;
    GIT_PACKAGE(std::string repo_name, std::string binary, std::string url) {
        os = false;
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
};

extern std::map<PACKAGE_MANAGER, std::string> install_commands;

class DependencyManager {
private:
    LOCAL_CONFIG* config;
    std::string work_dir;
    CommandExecutor* executor;
    std::string password;
public:
    DependencyManager(LOCAL_CONFIG* config, std::string work_dir) : config(config), work_dir(work_dir) {
        executor = new CommandExecutor(work_dir);
    };
    void set_password(std::string password);
    bool is_package_installed(PACKAGE* package);
    bool install_package(OS_PACKAGE* package);
    bool install_package(GIT_PACKAGE* package);
};

#endif //CERBERUS_DEPENDENCY_MANAGER_H
