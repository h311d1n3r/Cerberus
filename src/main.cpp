#include <utils/arg_parser.h>
#include <utils/config.h>
#include <langs/lang_manager.h>
#include <binaries/bin_identifier.h>
#include <utils/logger.h>
#include <global_defs.h>
#include <user/user_prompt.h>

using namespace std;

int main(int argc, char *argv[]) {
    ArgParser parser;
    CONFIG* config = parser.compute_args(argc, argv);
    if(config) {
        fcout << "$(bright_red:b)---------- $(red:b)" << TOOL_NAME << " (v" << TOOL_VERSION << ")$ ----------" << endl;
        BIN_TYPE type = identify_binary(config->binary_path);
        if(type == BIN_TYPE::UNKNOWN_TYPE) {
            fcout << "$(critical)The input file $(critical:u)" << config->binary_path << "$ couldn't be recognized..." << endl;
            return 1;
        }
        fcout << "$(info)Identified file as $(info:b)" << bin_type_names[type] << "$." << endl;
        LangIdentifier identifier(config->binary_path);
        value_ordered_map langs = identifier.identify();
        LANG selected_lang = LANG::UNKNOWN_LANG;
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
    }
    return 0;
}
