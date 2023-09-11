#include <user/local_config.h>
#include <unistd.h>

using namespace std;

map<PACKAGE_MANAGER, string> name_from_package_manager = {
    {UNKNOWN, "your package manager"},
    {APT, "apt"},
    {APT_GET, "apt-get"},
    {DNF, "dnf"},
    {YUM, "yum"},
    {ZYPPER, "zypper"},
    {PACMAN, "pacman"},
    {PORTAGE, "emerge"},
    {SLACKPKG, "slackpkg"},
    {SWARET, "swaret"},
    {XBPS, "xbps-install"},
    {APK, "apk"},
    {NIX, "nix-env"},
    {PETGET, "petget"}
};

PACKAGE_MANAGER find_package_manager() {
    for(pair<PACKAGE_MANAGER, string> manager : name_from_package_manager) {
        if(is_binary_on_path(manager.second)) return manager.first;
    }
    return PACKAGE_MANAGER::UNKNOWN;
}

bool is_root() {
    return geteuid() == 0;
}

bool has_sudo() {
    return is_binary_on_path("sudo");
}

LOCAL_CONFIG* identify_local_config() {
    LOCAL_CONFIG* config = new LOCAL_CONFIG();
    config->package_manager = find_package_manager();
    config->is_root = is_root();
    config->has_sudo = has_sudo();
    return config;
}

bool is_binary_on_path(string binary) {
    const char* path_env = std::getenv("PATH");
    if (path_env == nullptr) return false;
    string path_env_str(path_env);
    size_t start = 0;
    size_t end = path_env_str.find(':', start);
    while (end != string::npos) {
        string path = path_env_str.substr(start, end - start);
        string full_path = path + '/' + binary;
        if (access(full_path.c_str(), X_OK) == 0) return true;
        start = end + 1;
        end = path_env_str.find(':', start);
    }
    return false;
}