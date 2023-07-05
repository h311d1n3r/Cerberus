import logging
import os
import sys
import hashlib
import params
import re
import subprocess
from lief import ELF
from log import LogFormatter
from abc import ABC, abstractmethod
from shutil import which

class AbstractELFHandler(ABC):
    
    def __init__(self, elf_path):
        self.libs = {}
        self.md5_hashes = {}
        self.part_hashes = {}
        self.part_matches = {}
        self.matches = {}
        self.funcs = []
        if os.path.exists(elf_path):
            self.elf_path = elf_path
            self.elf = ELF.parse(elf_path)
            if not self.request_radare2_analysis():
                logging.error('radare2 analysis failed...')
                sys.exit(1)
            self.is_stripped = True
            if self.elf.has_section('.symtab') or self.elf.has_section('.strtab'):
                self.is_stripped = False
                logging.error('ELF file doesn\'t seem to be stripped. Continuing execution may produce a corrupted ELF file.')
                usr_run_unstripped = input('\033[0;'+str(LogFormatter.FORMAT_COLORS[logging.WARNING])+'m'+LogFormatter.FORMAT_PREFIXES[logging.WARNING]+'Continue anyway ? (y/N): ').strip()
                if not usr_run_unstripped.lower().startswith('y'):
                    sys.exit(1)
            self.elf_arch = self.elf.header.machine_type
            if self.elf_arch == ELF.ARCH.x86_64 or self.elf_arch == ELF.ARCH.i386:
                self.search_libs()
            else:
                logging.error('Unsupported architecture detected in ELF header.')
                sys.exit(1)
        else:
            logging.error('ELF file \033[0;'+str(LogFormatter.LOG_COLORS['WHITE'])+'m'+
                elf_path+'\033[0;'+str(LogFormatter.FORMAT_COLORS[logging.ERROR])+'m doesn\'t exist.')
            sys.exit(1)

    @abstractmethod
    def search_libs(self):
        pass
        
    @abstractmethod
    def download_and_build_libs(self, session_dir):
        pass

    def request_radare2_analysis(self):
        if which('radare2') is None:
            logging.warning('In-depth analysis with \033[1;'+str(LogFormatter.FORMAT_COLORS[logging.INFO])+'mradare2\033[0;'+str(LogFormatter.FORMAT_COLORS[logging.INFO])+'m is required for this program. Please install it and run again.')
            return False
        logging.info('In-depth analysis with \033[1;'+str(LogFormatter.FORMAT_COLORS[logging.INFO])+'mradare2\033[0;'+str(LogFormatter.FORMAT_COLORS[logging.INFO])+'m...')
        r2_out_name = '/tmp/r2_out_'+str(os.getpid())+'.txt'
        r2_out_f = open(r2_out_name, 'w')
        r2_proc = subprocess.Popen(['radare2', '-q', '-c', 'aaa', '-c', 'afl', self.elf_path], stdout=r2_out_f, stderr=subprocess.PIPE)
        r2_proc.wait()
        r2_proc.communicate()
        r2_out_f.close()
        Func = type('Func', (), {'address': 0, 'size': 0, 'name': None})
        r2_out_f = open(r2_out_name, 'r')
        r2_out = r2_out_f.read()
        r2_out_f.close()
        os.remove(r2_out_name)
        for line in r2_out.split('\n'):
            if line[0:2] != '0x':
                continue
            vals = line.split(' ')
            vals = [s for s in vals if s != '']
            fun_start = int(vals[0], 16)
            fun_sz = int(vals[2], 10)
            func = Func()
            func.address = fun_start - self.elf.imagebase
            func.size = fun_sz
            self.funcs.append(func)
        if len(self.funcs) == 0:
            return False
        return True

    def gen_part_hash(self, data):
        part_hash_pace = len(data) // params.PART_HASH_LEN
        if part_hash_pace < 1:
            part_hash_pace = 1
        part_hash = []
        for i in range(0, len(data), part_hash_pace):
            part_hash.append(data[i])
        return part_hash
    
    def compare_part_hashes(self, part_hash_1, part_hash_2):
        if len(part_hash_1) != len(part_hash_2):
            return 0
        score = 0
        for i in range(len(part_hash_1)):
            if part_hash_1[i] == part_hash_2[i]:
                score += 1
        return score / len(part_hash_1)
    
    def gen_hashes(self, session_dir):
        logging.info('Generating hash file for ELF target...')
        with open(self.elf_path, 'rb') as elf_file:
            elf_content = elf_file.read()
            elf_file.close()
        for func in self.funcs:
            if func.size >= params.MIN_FUNC_SIZE:
                func_data = elf_content[func.address:func.address+func.size]
                md5_hash = hashlib.md5(func_data).digest()
                self.md5_hashes[md5_hash] = func.address
                part_hash = self.gen_part_hash(func_data)
                if func.size not in self.part_hashes:
                    self.part_hashes[func.size] = {}
                self.part_hashes[func.size][func.address] = part_hash
        logging.success('Done ! Hashed '+str(len(self.md5_hashes))+' functions.')
    
    def demangle_name(self, name):
        args = ['c++filt', name]
        pipe = subprocess.Popen(args, stdin=subprocess.PIPE, stdout=subprocess.PIPE)
        stdout, _ = pipe.communicate()
        return stdout[0:len(stdout)-1].decode()
    
    @abstractmethod
    def get_lib_path(self, lib_dir):
        pass
    
    def compare_hashes(self, session_dir):
        logging.info('Checking for matches in built dependencies...')
        for lib_dir in os.listdir(session_dir):
            release_path = session_dir+'/'+self.get_lib_path(lib_dir)
            if os.path.exists(release_path):
                for release_file_name in os.listdir(release_path):
                    if release_file_name.endswith(".so"):
                        lib_elf = ELF.parse(release_path+'/'+release_file_name)
                        with open(release_path+'/'+release_file_name, 'rb') as elf_file:
                            elf_content = elf_file.read()
                            elf_file.close()
                        for func in lib_elf.functions:
                            if func.size >= params.MIN_FUNC_SIZE:
                                func_data = elf_content[func.address-lib_elf.imagebase:func.address-lib_elf.imagebase+func.size]
                                md5_hash = hashlib.md5(func_data).digest()
                                func_name = ''
                                if func.name == None or len(func.name) == 0:
                                    lib_name = re.split('\d+', lib_dir)[0]
                                    lib_name = lib_name[0:len(lib_name)-1]
                                    func_name = lib_name+'_'+hex(func.address)[2:]
                                else:
                                    func_name = self.demangle_name(func.name)
                                if md5_hash in self.md5_hashes:
                                    self.matches[self.md5_hashes[md5_hash]] = func_name
                                else:
                                    if func.size in self.part_hashes:
                                        part_hash = self.gen_part_hash(func_data)
                                        sized_part_hashes = self.part_hashes[func.size]
                                        for func_address in sized_part_hashes:
                                            func_address += self.elf.imagebase
                                            if func_address in self.matches:
                                                continue
                                            target_part_hash = sized_part_hashes[func_address - self.elf.imagebase]
                                            part_hash_score = self.compare_part_hashes(target_part_hash, part_hash)
                                            if part_hash_score >= params.PART_HASH_TRUST:
                                                if func_address not in self.part_matches:
                                                    self.part_matches[func_address] = [part_hash_score, func_name]
                                                else:
                                                    max_score, _ = self.part_matches[func_address]
                                                    if part_hash_score > max_score:
                                                        self.part_matches[func_address] = [part_hash_score, func_name]
                        for func_address in self.part_matches:
                            if func_address in self.matches:
                                continue
                            _, func_name = self.part_matches[func_address]
                            self.matches[func_address] = func_name
            else:
                logging.debug(release_path + ' doesn\'t exist.')
        logging.success('Done ! '+str(len(self.matches))+' matches.')

    def patch_elf(self):
        logging.info('Patching to new ELF...')
        if not self.elf.has_section('.symtab'):
            symtab_section             = ELF.Section()
            symtab_section.name        = ".symtab"
            symtab_section.type        = ELF.SECTION_TYPES.SYMTAB
            symtab_section.entry_size  = 0x18
            symtab_section.alignment   = 8
            symtab_section.link        = len(self.elf.sections) + 1
            symtab_section.content     = [0] * 100
            symtab_section = self.elf.add(symtab_section, loaded=False)
        else:
            symtab_section = self.elf.get_section('.symtab')
            
        if not self.elf.has_section('.strtab'):
            symstr_section            = ELF.Section()
            symstr_section.name       = ".strtab"
            symstr_section.type       = ELF.SECTION_TYPES.STRTAB
            symstr_section.entry_size = 1
            symstr_section.alignment  = 1
            symstr_section.content    = [0] * 100
            symstr_section = self.elf.add(symstr_section, loaded=False)
        else:
            symstr_section = self.elf.get_section('.strtab')

        sym_addresses = []
        if self.is_stripped:
            symbol         = ELF.Symbol()
            symbol.name    = ""
            symbol.type    = ELF.SYMBOL_TYPES.NOTYPE
            symbol.value   = 0
            symbol.binding = ELF.SYMBOL_BINDINGS.LOCAL
            symbol.size    = 0
            symbol.shndx   = 0
            symbol         = self.elf.add_static_symbol(symbol)
        else:
            for sym in self.elf.symbols:
                if sym.value not in sym_addresses:
                    sym_addresses.append(sym.value)
        
        for match_addr in self.matches:
            if self.is_stripped or match_addr not in sym_addresses:
                match_name = self.matches[match_addr]
                symbol         = ELF.Symbol()
                symbol.name    = match_name
                symbol.type    = ELF.SYMBOL_TYPES.FUNC
                symbol.value   = match_addr
                symbol.binding = ELF.SYMBOL_BINDINGS.LOCAL
                symbol.shndx   = 14
                symbol         = self.elf.add_static_symbol(symbol)
        
        self.elf.write(params.OUTPUT)
        logging.success('Done !')
