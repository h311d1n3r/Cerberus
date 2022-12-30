import os
import logging
import sys
import re
import urllib.request
import gzip
import shutil
import tarfile
import subprocess
import hashlib
from lief import ELF
from log import LogFormatter
from build_fixer import BuildFixer

MIN_FUNC_SIZE = 10
PART_HASH_LEN = 20
PART_HASH_TRUST = 0.6

class ELFHandler:
    
    def __init__(self, elf_path):
        self.crates = {}
        self.md5_hashes = {}
        self.part_hashes = {}
        self.part_matches = {}
        self.matches = {}
        if os.path.exists(elf_path):
            self.elf_path = elf_path
            self.elf = ELF.parse(elf_path)
            if self.elf.has_section('.symtab') or self.elf.has_section('.strtab'):
                logging.error('ELF file doesn\'t seem to be stripped.')
            self.elf_arch = self.elf.header.machine_type
            if self.elf_arch == ELF.ARCH.x86_64 or self.elf_arch == ELF.ARCH.i386:
                with open(elf_path, 'rb') as elf_file:
                    elf_content = elf_file.read()
                    crate_matches = re.findall(b'/.cargo/(.+?)\.rs', elf_content)
                    crate_matches.extend(re.findall(b'/cargo/(.+?)\.rs', elf_content))
                    for crate_match in crate_matches:
                        if b'\x00' in crate_match:
                            crate_match = crate_match[:crate_match.find(b'\x00')]
                        crate = crate_match.split(b'/')[3].decode()
                        crate_name = crate[:crate.rfind('-')]
                        crate_version = crate[len(crate_name)+1:]
                        if crate_name not in self.crates:
                            self.crates[crate_name] = crate_version
                elf_file.close()
            else:
                logging.error('Unsupported architecture detected in ELF header.')
                sys.exit(1)
        else:
            logging.error('ELF file \033[0;'+str(LogFormatter.LOG_COLORS['WHITE'])+'m'+
                elf_path+'\033[0;'+str(LogFormatter.FORMAT_COLORS[logging.ERROR])+'m doesn\'t exist.')
            sys.exit(1)

    def check_architecture_installation(self, architecture):
        installed_list = subprocess.run(['rustup','target','list'], capture_output=True)
        installed_list = installed_list.stdout.decode().split('\n')
        for installed_line in installed_list:
            if architecture in installed_line:
                if 'installed' not in installed_line:
                    logging.info('Architecture \033[0;'+str(LogFormatter.LOG_COLORS['WHITE'])+'m'+architecture+'\033[0;'+
                        str(LogFormatter.FORMAT_COLORS[logging.INFO])+'m doesn\'t seem to be installed.')
                    usr_install_arch = input('\033[0;'+str(LogFormatter.FORMAT_COLORS[logging.INFO])+'m'+
                        LogFormatter.FORMAT_PREFIXES[logging.INFO]+'Install architecture ? (Y/n): ').strip()
                    if not usr_install_arch.lower().startswith('n'):
                        logging.info('Proceeding to installation...')
                        try:
                            subprocess.run(['rustup','target','install',architecture], capture_output=True)
                            logging.success('Done !')
                        except:
                            logging.error('An error occured when installing architecture')
                break

    def download_and_build_crates(self, session_dir):
        logging.info('Downloading '+str(len(self.crates))+' crates...')
        for crate_name in self.crates:
            crate_version = self.crates[crate_name]
            crate_url = 'https://crates.io/api/v1/crates/'+crate_name+'/'+crate_version+'/download'
            urllib.request.urlretrieve(crate_url, session_dir+'/'+crate_name)
        logging.success('Done !')
        logging.info('Extracting crates from their archives...')
        for archive_name in os.listdir(session_dir):
            with gzip.open(session_dir+'/'+archive_name, 'rb') as gzip_archive:
                with open(session_dir+'/'+archive_name+'.crate', 'wb') as crate_archive:
                    shutil.copyfileobj(gzip_archive, crate_archive)
                    crate_archive.close()
                gzip_archive.close()
            os.remove(session_dir+'/'+archive_name)
        for crate_archive in os.listdir(session_dir):
            with tarfile.open(session_dir+'/'+crate_archive, 'r:') as tar_archive:
                tar_archive.extractall(session_dir)
                tar_archive.close()
            os.remove(session_dir+'/'+crate_archive)
        logging.success('Done !')
        logging.info('Patching Cargo.toml files to produce shared libraries...')
        for crate_dir in os.listdir(session_dir):
            cargo_file_path = session_dir+'/'+crate_dir+'/'+'Cargo.toml'
            if os.path.exists(cargo_file_path):
                new_cargo_file_lines = []
                with open(cargo_file_path, 'r') as cargo_file:
                    cargo_file_lines = cargo_file.readlines()
                    found_lib = False
                    for line in cargo_file_lines:
                        line = line.strip()
                        if '[lib]' in line:
                            found_lib = True
                            new_cargo_file_lines.append('[lib]\n')
                            new_cargo_file_lines.append('crate-type = ["dylib"]\n')
                        elif not line.startswith('crate-type ') and not line.startswith('crate-type='):
                            new_cargo_file_lines.append(line+'\n')
                    if not found_lib:
                        new_cargo_file_lines.append('\n[lib]\n')
                        new_cargo_file_lines.append('crate-type = ["dylib"]\n')
                    cargo_file.close()
                with open(cargo_file_path, 'w') as cargo_file:
                    cargo_file.writelines(new_cargo_file_lines)
                    cargo_file.close()
            else:
                logging.debug(cargo_file_path + ' doesn\'t exist.')
        logging.success('Done !')
        logging.info('Building crates...')
        built_crates_ctr = 0
        try:
            version_output = subprocess.run(['cargo', '--version'], capture_output=True)
        except:
            logging.fatal('Cargo is not installed !')
            return False
        if version_output.stdout.startswith(b'cargo'):
            for crate_dir in os.listdir(session_dir):
                build_params = ['cargo','build','--release']
                if self.elf_arch == ELF.ARCH.i386:
                    build_params.append('--target=i686-unknown-linux-gnu')
                    self.check_architecture_installation('i686-unknown-linux-gnu')
                try:
                    build_output = subprocess.run(build_params, cwd=session_dir+'/'+crate_dir, capture_output=True)
                    built_crates_ctr += 1
                except Exception as e:
                    logging.error('An error occured when building crate \033[0;'+
                        str(LogFormatter.LOG_COLORS['WHITE'])+'m'+crate_dir)
                    logging.debug(e)
                    continue
                build_err = build_output.stderr
                if build_err is not None:
                    build_err = build_err.split(b'\n')
                    is_error = False
                    for err_line in build_err:
                        if err_line.decode().strip().startswith('error: '):
                            is_error = True
                    if is_error:
                        logging.error('An error occured when building crate \033[0;'+
                            str(LogFormatter.LOG_COLORS['WHITE'])+'m'+crate_dir)
                        logging.debug('Here is the trace :')
                        if logging.getLogger(__name__).getEffectiveLevel() <= logging.DEBUG:
                            for err_line in build_err:
                                print('\033[0;'+str(LogFormatter.FORMAT_COLORS[logging.DEBUG])+'m'+err_line.decode())
                        logging.info('Delegating to BuildFixer...')
                        build_fixer = BuildFixer(session_dir+'/'+crate_dir, build_err, self.elf_arch)
                        if not build_fixer.success:
                            built_crates_ctr -= 1
            logging.success('Done ! ('+str(built_crates_ctr)+'/'+str(len(self.crates))+')')
            return True
        else:
            logging.fatal('Error when using cargo command !')
        return False

    def gen_part_hash(self, data):
        part_hash_pace = len(data) // PART_HASH_LEN
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
        for func in self.elf.functions:
            if func.size >= MIN_FUNC_SIZE:
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
    
    def compare_hashes(self, session_dir):
        logging.info('Checking for matches in built crates...')
        for crate_dir in os.listdir(session_dir):
            release_path = session_dir+'/'+crate_dir+'/target/release'
            if self.elf_arch == ELF.ARCH.i386:
                release_path = session_dir+'/'+crate_dir+'/target/i686-unknown-linux-gnu/release'
            if os.path.exists(release_path):
                for release_file_name in os.listdir(release_path):
                    if release_file_name.endswith(".so"):
                        lib_elf = ELF.parse(release_path+'/'+release_file_name)
                        with open(release_path+'/'+release_file_name, 'rb') as elf_file:
                            elf_content = elf_file.read()
                            elf_file.close()
                        for func in lib_elf.functions:
                            if func.size >= MIN_FUNC_SIZE:
                                func_data = elf_content[func.address:func.address+func.size]
                                md5_hash = hashlib.md5(func_data).digest()
                                func_name = ''
                                if func.name == None or len(func.name) == 0:
                                    crate_name = re.split('\d+', crate_dir)[0]
                                    crate_name = crate_name[0:len(crate_name)-1]
                                    func_name = crate_name+'_'+hex(func.address)[2:]
                                else:
                                    func_name = self.demangle_name(func.name)
                                if md5_hash in self.md5_hashes:
                                    self.matches[self.md5_hashes[md5_hash]] = func_name
                                else:
                                    if func.size in self.part_hashes:
                                        part_hash = self.gen_part_hash(func_data)
                                        sized_part_hashes = self.part_hashes[func.size]
                                        for func_address in sized_part_hashes:
                                            if func_address in self.matches:
                                                continue
                                            target_part_hash = sized_part_hashes[func_address]
                                            part_hash_score = self.compare_part_hashes(target_part_hash, part_hash)
                                            if part_hash_score >= PART_HASH_TRUST:
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

    def patch_elf(self, out_path):
        logging.info('Patching to new ELF...')
        symtab_section             = ELF.Section()
        symtab_section.name        = ".symtab"
        symtab_section.type        = ELF.SECTION_TYPES.SYMTAB
        symtab_section.entry_size  = 0x18
        symtab_section.alignment   = 8
        symtab_section.link        = len(self.elf.sections) + 1
        symtab_section.content     = [0] * 100

        symstr_section            = ELF.Section()
        symstr_section.name       = ".strtab"
        symstr_section.type       = ELF.SECTION_TYPES.STRTAB
        symstr_section.entry_size = 1
        symstr_section.alignment  = 1
        symstr_section.content    = [0] * 100
        
        symtab_section = self.elf.add(symtab_section, loaded=False)
        symstr_section = self.elf.add(symstr_section, loaded=False)
        
        symbol         = ELF.Symbol()
        symbol.name    = ""
        symbol.type    = ELF.SYMBOL_TYPES.NOTYPE
        symbol.value   = 0
        symbol.binding = ELF.SYMBOL_BINDINGS.LOCAL
        symbol.size    = 0
        symbol.shndx   = 0
        symbol         = self.elf.add_static_symbol(symbol)
        
        for match_addr in self.matches:
            match_name = self.matches[match_addr]
            symbol         = ELF.Symbol()
            symbol.name    = match_name
            symbol.type    = ELF.SYMBOL_TYPES.FUNC
            symbol.value   = match_addr
            symbol.binding = ELF.SYMBOL_BINDINGS.LOCAL
            symbol.shndx   = 14
            symbol         = self.elf.add_static_symbol(symbol)
        
        self.elf.write(out_path)
        logging.success('Done !')
