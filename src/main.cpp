#include <utils/arg_parser.h>
#include <utils/config.h>
#include <langs/lang_manager.h>

using namespace std;

int main(int argc, char *argv[]) {
    ArgParser parser;
    CONFIG* config = parser.compute_args(argc, argv);
    if(config) {
        LangIdentifier identifier(config->binary_path);
        identifier.identify();
    }
    return 0;
}
