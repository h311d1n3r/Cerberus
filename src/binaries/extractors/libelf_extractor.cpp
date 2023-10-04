#include <binaries/extractors/libelf_extractor.h>
#include <fcntl.h>
#include <unistd.h>

using namespace std;

LibelfExtractor::LibelfExtractor(std::string bin_path, BIN_TYPE type) : BinaryExtractor(bin_path, type) {
    this->fd = open(bin_path.c_str(), O_RDONLY, 0);
    this->fp = fdopen(fd, "r");
    this->bin = elf_begin(fd, ELF_C_READ, NULL);
}

LibelfExtractor::~LibelfExtractor() {
    elf_end(this->bin);
    close(fd);
}

BIN_ARCH LibelfExtractor::extract_arch() {

}

vector<unique_ptr<FUNCTION>> LibelfExtractor::extract_functions_32(size_t image_base) {
    vector<unique_ptr<FUNCTION>> funcs;
    Elf_Scn *scn = nullptr;
    Elf_Scn *sym_scn = nullptr;
    uint64_t str_scn_start = 0;
    while ((scn = elf_nextscn(this->bin, scn)) != nullptr) {
        Elf32_Shdr *shdr = elf32_getshdr(scn);
        if(!sym_scn) {
            if(shdr->sh_type == SHT_SYMTAB || shdr->sh_type == SHT_DYNSYM) {
                sym_scn = scn;
                if(str_scn_start) break;
            }
        }
        if(!str_scn_start) {
            if(shdr->sh_type == SHT_STRTAB) {
                str_scn_start = shdr->sh_addr;
                if(sym_scn) break;
            }
        }
    }
    if(sym_scn) {
        Elf_Data *data = nullptr;
        data = elf_getdata(sym_scn, data);
        Elf32_Sym *symtab = (Elf32_Sym*) data->d_buf;
        size_t sym_count = data->d_size / sizeof(Elf32_Sym);
        for (size_t i = 0; i < sym_count; i++) {
            if(ELF32_ST_TYPE(symtab[i].st_info) != STT_FUNC || !symtab[i].st_size) continue;
            unique_ptr<FUNCTION> func = make_unique<FUNCTION>();
            func->start = symtab[i].st_value - image_base;
            func->end = func->start + symtab[i].st_size - 1;
            if(!str_scn_start || !symtab[i].st_value) {
                funcs.push_back(move(func));
                continue;
            }
            char buf[1024];
            fseek(this->fp, symtab[i].st_name-image_base+str_scn_start, SEEK_SET);
            fread(buf, 1024, 1, this->fp);
            func->name = string(buf);
            funcs.push_back(move(func));
        }
    }
    return funcs;
}

vector<unique_ptr<FUNCTION>> LibelfExtractor::extract_functions_64(size_t image_base) {
    vector<unique_ptr<FUNCTION>> funcs;
    Elf_Scn *scn = nullptr;
    Elf_Scn *sym_scn = nullptr;
    uint64_t str_scn_start = 0;
    while ((scn = elf_nextscn(this->bin, scn)) != nullptr) {
        Elf64_Shdr *shdr = elf64_getshdr(scn);
        if(!sym_scn) {
            if(shdr->sh_type == SHT_SYMTAB || shdr->sh_type == SHT_DYNSYM) {
                sym_scn = scn;
                if(str_scn_start) break;
            }
        }
        if(!str_scn_start) {
            if(shdr->sh_type == SHT_STRTAB) {
                str_scn_start = shdr->sh_addr;
                if(sym_scn) break;
            }
        }
    }
    if(sym_scn) {
        Elf_Data *data = nullptr;
        data = elf_getdata(sym_scn, data);
        Elf64_Sym *symtab = (Elf64_Sym*) data->d_buf;
        size_t sym_count = data->d_size / sizeof(Elf64_Sym);
        for (size_t i = 0; i < sym_count; i++) {
            if(ELF64_ST_TYPE(symtab[i].st_info) != STT_FUNC || !symtab[i].st_size) continue;
            unique_ptr<FUNCTION> func = make_unique<FUNCTION>();
            func->start = symtab[i].st_value - image_base;
            func->end = func->start + symtab[i].st_size - 1;
            if(!str_scn_start || !symtab[i].st_value) {
                funcs.push_back(move(func));
                continue;
            }
            char buf[1024];
            fseek(this->fp, symtab[i].st_name-image_base+str_scn_start, SEEK_SET);
            fread(buf, 1024, 1, this->fp);
            func->name = string(buf);
            funcs.push_back(move(func));
        }
    }
    return funcs;
}

vector<unique_ptr<FUNCTION>> LibelfExtractor::extract_functions(BIN_ARCH arch, size_t image_base) {
    if(type==BIN_TYPE::PE) image_base = 0;
    vector<unique_ptr<FUNCTION>> funcs;
    switch(arch) {
        case BIN_ARCH::X86_64:
            funcs = extract_functions_64(image_base);
            break;
        case BIN_ARCH::X86:
            funcs = extract_functions_32(image_base);
            break;
    }
    return funcs;
}

vector<unique_ptr<SECTION>> LibelfExtractor::extract_sections() {

}
