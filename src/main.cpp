#include <utils/arg_parser.h>
#include <utils/config.h>

using namespace std;

int main(int argc, char *argv[]) {
    ArgParser parser;
    CONFIG* config = parser.compute_args(argc, argv);
    if(config) {

    }
    return 0;
}
