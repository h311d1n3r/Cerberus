#include <utils/arg_parser.h>
#include <utils/config.h>
#include <langs/lang_manager.h>
#include <binaries/bin_identifier.h>
#include <utils/logger.h>
#include <global_defs.h>
#include <user/user_prompt.h>
#include <binaries/handlers/elf_handler.h>
#include <binaries/handlers/pe_handler.h>

using namespace std;

CONFIG* config;
LANG selected_lang = LANG::UNKNOWN_LANG;
BIN_TYPE type;

void start_analysis() {
    BinaryHandler* handler;
    switch(type) {
        case PE:
            handler = new PeHandler(config->binary_path);
            break;
        case ELF:
            handler = new ElfHandler(config->binary_path);
            break;
        default:
            return;
    }
    handler->strip_analysis();
    if(handler->is_stripped()) fcout << "$(info)File was found to be $(info:b)stripped$." << endl;
    else {
        fcout << "$(warning)File was not found to be $(warning:b)stripped$..." << endl;
        if(!ask_yes_no("Resume analysis ?", true)) return;
    }
    fcout << "$(info)Extracting libraries..." << endl;
    handler->libs_extraction(selected_lang);
}

int main(int argc, char *argv[]) {
    ArgParser parser;
    config = parser.compute_args(argc, argv);
    if(config) {
        fcout << "$(bright_red:b)---------- $(red:b)" << TOOL_NAME << " (v" << TOOL_VERSION << ")$ ----------" << endl;
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
            fcout << "$$(info)Currently supported languages :\n";
            for(size_t lang_i = 0; lang_i < langs.size(); lang_i++) {
                fcout << "$(magenta)" << to_string(lang_i+1) << ". $(magenta:b)" << name_from_lang[langs.at(lang_i).first] << endl;
            }
            uint8_t selected_lang_i = ask_n("$(info)Your choice ?", 1, langs.size());
            selected_lang = langs.at(selected_lang_i-1).first;
        }
        fcout << "$$(info)Using $(magenta:b)" << name_from_lang[selected_lang] << "$ for analysis." << endl;
        start_analysis();
    }
    return 0;
}
