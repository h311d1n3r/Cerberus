#include <binaries/lib/lib_manager.h>
#include <utils/logger.h>
#include <user/user_prompt.h>
#include <iostream>

using namespace std;

void LibManager::main_menu() {
    fcout << "$(info)Here is the current list of libraries :" << endl;
    uint8_t lib_i = 1;
    for(LIBRARY* lib : this->libs) {
        fcout << "$(info)" << to_string(lib_i) << ". $(bright_magenta:b)" << lib->name << "$$(red):$$(magenta:b)" << lib->version << endl;
        lib_i++;
    }
    fcout << "$(info)$(info:b)1$. Validate $(info:b)2$. Add library $(info:b)3$. Change library version $(info:b)4$. Remove library" << endl;
}

void LibManager::add_lib_menu() {
    LIBRARY* lib = new LIBRARY;
    lib->name = "";
    lib->version = "";
    fcout << "$(info)Name:$ ";
    while(!lib->name.size()) getline(cin, lib->name);
    fcout << "$(info)Version:$ ";
    while(!lib->version.size()) getline(cin, lib->version);
    this->libs.push_back(lib);
}

void LibManager::change_version_menu() {
    if(!this->libs.size()) {
        fcout << "$(warning)No libraries remaining." << endl;
        return;
    }
    size_t index = ask_n("Index:", 1, this->libs.size());
    LIBRARY* lib = this->libs.at(index - 1);
    lib->version = "";
    fcout << "$(info)Version:$ ";
    while(!lib->version.size()) getline(cin, lib->version);
}

void LibManager::remove_lib_menu() {
    if(!this->libs.size()) {
        fcout << "$(warning)No libraries remaining." << endl;
        return;
    }
    size_t index = ask_n("Index:", 1, this->libs.size());
    this->libs.erase(this->libs.begin() + index - 1);
}

void LibManager::manage() {
    USER_CHOICE choice = USER_CHOICE::NO_CHOICE;
    do {
        main_menu();
        choice = (USER_CHOICE) ask_n("Your choice ?", 1, 4);
        switch(choice) {
            case ADD_LIB:
                this->add_lib_menu();
                break;
            case CHANGE_VERSION:
                this->change_version_menu();
                break;
            case REMOVE_LIB:
                this->remove_lib_menu();
                break;
        }
    } while(choice != USER_CHOICE::VALIDATE);
}