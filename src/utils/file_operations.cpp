#include <utils/file_operations.h>
#include <fstream>
#include <gzip/utils.hpp>
#include <gzip/decompress.hpp>
#include <archive.h>
#include <archive_entry.h>
#include <fcntl.h>
#include <filesystem>
#include <iostream>

using namespace std;

bool decompress_gzip_file(string input, string output) {
    ifstream input_file(input, ios::binary);
    input_file.seekg(0, ios::end);
    size_t input_file_sz = input_file.tellg();
    input_file.seekg(0);
    char data[input_file_sz];
    input_file.read(data, input_file_sz);
    input_file.close();
    if(!gzip::is_compressed(data, input_file_sz)) return false;
    string decompressed = gzip::decompress(data, input_file_sz);
    ofstream output_file(output, ios::binary);
    output_file.write(decompressed.c_str(), decompressed.size());
    output_file.close();
    return true;
}

bool decompress_tar_file(string input, string output) {
    struct archive* a = archive_read_new();
    archive_read_support_format_all(a);
    if (archive_read_open_filename(a, input.c_str(), 10240) != ARCHIVE_OK) {
        return false;
    }
    if (!filesystem::create_directory(output)) {
        return false;
    }
    struct archive_entry* entry;
    while (archive_read_next_header(a, &entry) == ARCHIVE_OK) {
        string entry_name = string(archive_entry_pathname(entry));
        size_t first_separator_index = entry_name.find('/');
        if(first_separator_index == string::npos) continue;
        entry_name = entry_name.substr(first_separator_index+1);
        string output_file_path = string(output) + "/" + entry_name;
        size_t last_separator_index = output_file_path.rfind('/');
        if(last_separator_index != string::npos) {
            string directory_part = output_file_path.substr(0, last_separator_index);
            filesystem::create_directories(directory_part);
        }
        int outputFd = open(output_file_path.c_str(), O_WRONLY | O_CREAT | O_TRUNC, 0666);
        if (outputFd == -1) {
            continue;
        }
        const void* buffer;
        size_t size;
        int64_t offset;
        while (archive_read_data_block(a, &buffer, &size, &offset) == ARCHIVE_OK) {
            write(outputFd, buffer, size);
        }
        close(outputFd);
    }
    archive_read_close(a);
    archive_read_free(a);
    return true;
}