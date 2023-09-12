#ifndef CERBERUS_LIB_MANAGER_H
#define CERBERUS_LIB_MANAGER_H

#include <vector>
#include <binaries/bin_types.h>
#include <string>
#include <memory>

enum USER_CHOICE {
    NO_CHOICE = 0,
    VALIDATE = 1,
    ADD_LIB = 2,
    CHANGE_VERSION = 3,
    REMOVE_LIB = 4
};

class LibManager {
private:
    std::vector<std::unique_ptr<LIBRARY>>& libs;
    void main_menu();
    void add_lib_menu();
    void change_version_menu();
    void remove_lib_menu();
public:
    LibManager(std::vector<std::unique_ptr<LIBRARY>>& libs) : libs(libs) {}
    void manage();
};

#endif //CERBERUS_LIB_MANAGER_H
