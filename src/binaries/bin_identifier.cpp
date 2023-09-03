#include <binaries/bin_identifier.h>
#include <fstream>
#include <cstring>

using namespace std;

map<BIN_TYPE, string> bin_type_names {
    {UNKNOWN_TYPE, "Unknown"},
    {ELF, "UNIX - Executable and Linkable Format (ELF)"},
    {PE, "WINDOWS - Portable Executable (PE)"}
};

map<string, BIN_TYPE> bin_type_from_magic = {
    {std::string("\x7f")+"ELF", BIN_TYPE::ELF},
    {"MZ", BIN_TYPE::PE}
};

BIN_TYPE identify_binary(string input_path) {
    ifstream input_file(input_path, ios::binary);
    input_file.seekg(0, ios::end);
    size_t input_sz = input_file.tellg();
    for (const pair<string, BIN_TYPE> p : bin_type_from_magic) {
        input_file.seekg(0);
        uint8_t magic_length = p.first.length();
        if(input_sz >= magic_length) {
            char magic_buffer[magic_length];
            input_file.read(magic_buffer, magic_length);
            if(!strncmp(p.first.c_str(), magic_buffer, magic_length)) {
                input_file.close();
                return p.second;
            }
        }
    }
    input_file.close();
    return UNKNOWN_TYPE;
}