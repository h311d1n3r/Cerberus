#ifndef CERBERUS_LOCAL_CONFIG_H
#define CERBERUS_LOCAL_CONFIG_H

#include <string>
#include <map>

enum PACKAGE_MANAGER {
    UNKNOWN,
    APT,
    APT_GET,
    DNF,
    YUM,
    ZYPPER,
    PACMAN,
    PORTAGE,
    SLACKPKG,
    SWARET,
    XBPS,
    APK,
    NIX,
    PETGET
};

struct LOCAL_CONFIG {
    PACKAGE_MANAGER package_manager;
    bool has_internet;
    bool is_root;
    bool has_sudo;
};

extern std::map<PACKAGE_MANAGER, std::string> name_from_package_manager;

LOCAL_CONFIG* identify_local_config();

bool is_binary_on_path(std::string binary);

#endif //CERBERUS_LOCAL_CONFIG_H
