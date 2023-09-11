#include <utils/arg_parser.h>
#include <utils/config.h>
#include <langs/lang_manager.h>
#include <binaries/bin_identifier.h>
#include <utils/logger.h>
#include <global_defs.h>
#include <user/user_prompt.h>
#include <binaries/handlers/elf_handler.h>
#include <binaries/handlers/pe_handler.h>
#include <binaries/lib/lib_manager.h>
#include <user/local_config.h>
#include <filesystem>
#include <uuid/uuid.h>
#include <curl/curl.h>
#include "algorithm/part_hash_algorithm.h"

namespace fs = std::filesystem;
using namespace std;

std::string work_dir;
CONFIG* config;
LANG selected_lang = LANG::UNKNOWN_LANG;
BIN_TYPE type;
LOCAL_CONFIG* usr_config;

vector<PACKAGE*> packages = {
    new OS_PACKAGE{"git", "git"},
    new OS_PACKAGE{"cargo", "cargo"},
    new GIT_PACKAGE{"radare2", "radare2", "https://github.com/radareorg/radare2", 0, "cd .. ; mv radare2 ../ ; ../radare2/sys/install.sh", false},
};

void global_init() {
    curl_global_init(CURL_GLOBAL_ALL);
}

void global_exit() {
    curl_global_cleanup();
}

bool install_dependencies(BinaryHandler* handler) {
    DependencyManager dep_manager(usr_config, work_dir);
    std::vector<OS_PACKAGE*> os_packages;
    std::vector<GIT_PACKAGE*> git_packages;
    for(PACKAGE* package : packages) {
        if(!dep_manager.is_package_installed(package)) {
            if(package->os) os_packages.push_back((OS_PACKAGE*)package);
            else git_packages.push_back((GIT_PACKAGE*)package);
        }
    }
    if(!os_packages.size() && !git_packages.size()) fcout << "$(info)No additional package is required." << endl;
    else {
        fcout << "$(info)The following packages are required :" << endl;
        if(os_packages.size()) {
            fcout << "$(bright_magenta)With $(bright_magenta:b)" << name_from_package_manager[usr_config->package_manager] << "$:" << endl;
            for (OS_PACKAGE *package : os_packages) {
                fcout << "$(magenta)- $(magenta:b)" + package->name << endl;
            }
        }
        if(git_packages.size()) {
            fcout << "$(bright_red)With $(bright_red:b)git$:" << endl;
            for (GIT_PACKAGE *package : git_packages) {
                fcout << "$(red)- $(red:b)" + package->repo_name << endl;
            }
        }
        if(usr_config->is_root || usr_config->has_sudo) {
            if(usr_config->package_manager != PACKAGE_MANAGER::UNKNOWN || !os_packages.size()) {
                bool agrees_install = ask_yes_no("Proceed to installation ?", true);
                if(agrees_install) {
                    if(!usr_config->is_root) {
                        string password = ask_password("$(info)Input your password for sudo");
                        dep_manager.set_password(password);
                    }
                    for(OS_PACKAGE* package : os_packages) {
                        if(dep_manager.install_package(package)) fcout << "$(success)Done." << endl;
                        else {
                            fcout << "$(error)An error occurred during installation..." << endl;
                            return false;
                        }
                    }
                    for(GIT_PACKAGE* package : git_packages) {
                        if(dep_manager.install_package(package)) fcout << "$(success)Done." << endl;
                        else {
                            fcout << "$(error)An error occurred during installation..." << endl;
                            return false;
                        }
                    }
                } else {
                    fcout << "$(error)Ending execution." << endl;
                    return false;
                }
            } else {
                fcout << "$(error)Without a known package manager, please install these packages and try again." << endl;
                return false;
            }
        } else {
            fcout << "$(error)Without being root or having sudo, please manually install these packages and try again." << endl;
            return false;
        }
    }
    return true;
}

void start_analysis() {
    BinaryHandler* handler;
    Algorithm* algorithm = new PartHashAlgorithm(config);
    switch(type) {
        case PE:
            handler = new PeHandler(config->binary_path, work_dir, selected_lang, algorithm);
            break;
        case ELF:
            handler = new ElfHandler(config->binary_path, work_dir, selected_lang, algorithm);
            break;
        default:
            return;
    }
    if(!install_dependencies(handler)) return;
    handler->strip_analysis();
    if(handler->is_stripped()) fcout << "$(info)File was found to be $(info:b)stripped$." << endl;
    else {
        fcout << "$(warning)File was not found to be $(warning:b)stripped$..." << endl;
        if(!ask_yes_no("Resume analysis ?", true)) return;
    }
    fcout << "$(info)Extracting libraries..." << endl;
    size_t libs_amount = handler->libs_extraction();
    if(!libs_amount) {
        fcout << "$(error)No libraries were found." << endl;
        return;
    }
    fcout << "$(success)Identified $(success:b)" << to_string(libs_amount) << "$ libraries." << endl;
    std::vector<LIBRARY*> libs = handler->get_libs();
    LibManager lib_manager(libs);
    lib_manager.manage();
    if(!libs.size()) {
        fcout << "$(error)No libraries remaining." << endl;
        return;
    }
    fcout << "$(info)Installing libraries..." << endl;
    size_t libs_installed = handler->libs_installation();
    if(!libs_installed) {
        fcout << "$(error)No libraries were successfully installed..." << endl;
        return;
    }
    fcout << "$(success)Installed $(success:b)" << to_string(libs_installed) << "$ libraries." << endl;
    fcout << "$(info)Analyzing functions..." << endl;
    size_t funcs_sz = handler->functions_analysis();
    if(!funcs_sz) {
        fcout << "$(error)No functions were successfully analyzed..." << endl;
        return;
    }
    fcout << "$(success)Analyzed $(success:b)" << to_string(funcs_sz) << "$ functions." << endl;
}

std::string generate_work_dir() {
    uuid_t uuid;
    uuid_generate_random(uuid);
    char uuid_str[37];
    uuid_unparse_lower(uuid, uuid_str);
    work_dir = ".cerberus-"+string(uuid_str, 37).substr(0, 16);
    fs::create_directory(work_dir);
    return work_dir;
}

int main(int argc, char *argv[]) {
    global_init();
    ArgParser parser;
    config = parser.compute_args(argc, argv);
    if(config) {
        fcout << "$(bright_red:b)---------- $(red:b)" << TOOL_NAME << " (v" << TOOL_VERSION << ")$ ----------" << endl;
        usr_config = identify_local_config();
        if(usr_config->package_manager != PACKAGE_MANAGER::UNKNOWN) fcout << "$(info)Identified package manager: $(info:b)" << name_from_package_manager[usr_config->package_manager] << endl;
        else fcout << "$(warning)Could not identify package manager..." << endl;
        if(usr_config->is_root) fcout << "$(info)Running as root." << endl;
        else if(usr_config->has_sudo) fcout << "$(info)Not running as root but sudo detected." << endl;
        else fcout << "$(warning)Not running as root and sudo couldn't be detected." << endl;
        type = identify_binary(config->binary_path);
        if(type == BIN_TYPE::UNKNOWN_TYPE) {
            fcout << "$(critical)The input file $(critical:u)" << config->binary_path << "$ couldn't be recognized..." << endl;
            return 1;
        }
        fcout << "$(info)Identified file as $(info:b)" << bin_type_names[type] << "$." << endl;
        LangIdentifier identifier(config->binary_path);
        value_ordered_map langs = identifier.identify();
        if(langs.at(0).second) {
            fcout << "$(info)Identified language : $(magenta:b)" << name_from_lang[langs.at(0).first] << endl;
            bool agree_lang = ask_yes_no("Continue analysis with this language ?", true);
            if(agree_lang) selected_lang = langs.at(0).first;
        }
        if(selected_lang == LANG::UNKNOWN_LANG) {
            fcout << "$(info)Currently supported languages :\n";
            for(size_t lang_i = 0; lang_i < langs.size(); lang_i++) {
                fcout << "$(magenta)" << to_string(lang_i+1) << ". $(magenta:b)" << name_from_lang[langs.at(lang_i).first] << endl;
            }
            uint8_t selected_lang_i = ask_n("$(info)Your choice ?", 1, langs.size());
            selected_lang = langs.at(selected_lang_i-1).first;
        }
        work_dir = generate_work_dir();
        fcout << "$(info)Using $(magenta:b)" << name_from_lang[selected_lang] << "$ for analysis." << endl;
        start_analysis();
        //fs::remove_all(work_dir);
    }
    global_exit();
    return 0;
}
