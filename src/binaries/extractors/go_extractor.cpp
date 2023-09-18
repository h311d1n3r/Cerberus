#include <binaries/extractors/go_extractor.h>
#include <utils/convert.h>
#include <fstream>

using namespace std;

BIN_ARCH GoExtractor::extract_arch() {

}

vector<unique_ptr<FUNCTION>> GoExtractor::extract_functions(BIN_ARCH arch, size_t image_base) {
    ifstream bin_file(this->bin_path, ios::binary);
    LIEF::PE::Section* text_sec;
    if(type == BIN_TYPE::PE) text_sec = this->lief_extractor.extract_text_section();
    vector<unique_ptr<FUNCTION>> funcs;
    COMMAND_RESULT res;
    executor.execute_command("go tool nm -size "+this->bin_path, &res);
    if(res.code) return funcs;
    vector<string> lines = split_string(res.response, '\n');
    for(string line : lines) {
        vector<string> vals = split_string(line, ' ');
        vals = filter_empty_strings(vals);
        if(vals.size() < 4 || !isdigit(vals.at(0).at(0))) continue;
        unique_ptr<FUNCTION> func = make_unique<FUNCTION>();
        switch(type) {
            case BIN_TYPE::ELF:
                func->start = stoull(vals.at(0), nullptr, 16) - image_base;
                break;
            case BIN_TYPE::PE:
                func->start = stoull(vals.at(0), nullptr, 16) - image_base - text_sec->virtual_address() + text_sec->offset();
                break;
        }
        func->end = func->start + stoull(vals.at(1)) - 1;
        unsigned char b;
        bin_file.seekg(func->end);
        bin_file.read((char*)&b, 1);
        bool found_cc = false;
        if(b == 0xCC) found_cc = true;
        while(b == 0xCC) {
            bin_file.seekg(func->end--);
            bin_file.read((char*)&b, 1);
        }
        if(found_cc) func->end++;
        func->name = vals.at(3);
        if(!func->name.find("$f64") || !func->name.find("$f32")) func->name = "";
        funcs.push_back(move(func));
    }
    return funcs;
}

vector<unique_ptr<SECTION>> GoExtractor::extract_sections() {

}