#include <utils/arg_parser.h>
#include <utils/logger.h>
#include <global_defs.h>
#include <filesystem>

namespace fs = std::filesystem;
using namespace std;

string ArgParser::format_help() {
    string res = "$(cyan)$(cyan:b)" + TOOL_ART + "$\n";
    res += " Version: $(white:b)" + TOOL_VERSION + "$\n";
    res += " Author: $(white:b)" + TOOL_AUTHOR + "$\n";
    res += "______________________________________\n\n";
    res += "$(cyan:b)Syntax: $(red:b)cerberus binary [-param value] [--flag]$$\n\n";
    res += "$(cyan:b)Parameters:$\n";
    res += "   $(red:b)output (o)$ -> Specifies the path for the resulting executable file. $(cyan:b)Default value: [input_binary]-patched$\n";
    res += "   $(red:b)part_hash_len (phl)$ -> Specifies the length of a part hash. The part hash of a function is just a reduction of the function with a linear pace. This technique is used to prevent fixed addresses from corrupting a standard hash. $(cyan:b)Default value: 20$\n";
    res += "   $(red:b)part_hash_trust (pht)$ -> Specifies minimum ratio of similarity between the two hashed functions to compare. The kept function will be the one with the most matches anyway. Increasing this value will reduce the number of matched functions but speed up execution time. $(cyan:b)Default value: 0.6$\n";
    res += "   $(red:b)min_func_size (mfs)$ -> Specifies the minimum length a function must be to get analyzed. Decreasing this value will increase matches but also false positives. $(cyan:b)Default value : 10$\n\n";
    res += "$(cyan:b)Flags:$\n";
    res += "   $(red:b)help (h)$ -> Displays this message.\n";
    res += "   $(red:b)no-prompt (np)$ -> Automatically skips user prompts.";
    return res;
}

void ArgParser::prepare_args() {
    this->parser.add_argument("binary").default_value("");
    this->parser.add_argument("-output", "-o");
    this->parser.add_argument("-part_hash_len", "-phl");
    this->parser.add_argument("-part_hash_trust", "-pht");
    this->parser.add_argument("-min_func_size", "-mfs");
    this->parser.add_argument("--help", "--h").implicit_value(true);
    this->parser.add_argument("--no-prompt", "--np").implicit_value(true);
}

CONFIG* ArgParser::compute_args(int argc, char **argv) {
    CONFIG* config = new CONFIG;
    if(argc <= 2) {
        this->parser.parse_args(argc, argv);
        config->binary_path = this->parser.get<string>("binary");
        config->output_path = this->parser.is_used("-output") ? this->parser.get<string>("output") :
                              config->binary_path + "-patched";
        if (this->parser.is_used("-part_hash_len")) config->part_hash_len = this->parser.get<uint16_t>("part_hash_len");
        if (this->parser.is_used("-part_hash_trust"))
            config->part_hash_trust = this->parser.get<float>("part_hash_trust");
        if (this->parser.is_used("-min_func_size")) config->min_func_size = this->parser.get<uint16_t>("min_func_size");
        if (this->parser.is_used("--no-prompt")) config->no_prompt = true;
    }
    if(argc > 2 || this->parser.is_used("--help") || !config->binary_path.length()) {
        string help = this->format_help();
        fcout << help << endl;
        exit(0);
    }
    if(!fs::exists(config->binary_path)) {
        fcout << "$(critical)File $(critical:u)" << config->binary_path << "$ does not exist !" << endl;
        exit(1);
    }
    return config;
}

ArgParser::ArgParser() {
    this->parser = argparse::ArgumentParser("cerberus");
    this->prepare_args();
}