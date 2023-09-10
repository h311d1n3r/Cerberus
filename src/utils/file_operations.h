#ifndef CERBERUS_FILE_OPERATIONS_H
#define CERBERUS_FILE_OPERATIONS_H

#include <string>

bool decompress_gzip_file(std::string input, std::string output);
bool decompress_tar_file(std::string input, std::string output);

#endif //CERBERUS_FILE_OPERATIONS_H
