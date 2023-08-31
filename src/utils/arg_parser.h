#ifndef CERBERUS_ARG_PARSER_H
#define CERBERUS_ARG_PARSER_H

#include <utils/config.h>
#include <argparse/argparse.hpp>

class ArgParser {
private:
    argparse::ArgumentParser parser;
    std::string format_help();
    void prepare_args();
public:
    ArgParser();
    CONFIG* compute_args(int argc, char *argv[]);
};

#endif //CERBERUS_ARG_PARSER_H
