import re
import logging
import requests
import os
import gzip
import shutil
import tarfile
import subprocess
from lief import ELF
from log import LogFormatter
from build_fixer import BuildFixer
from abs_elf_handler import AbstractELFHandler

class ELFHandler(AbstractELFHandler):
    
    def __init__(self, elf_path):
        super().__init__(elf_path)
        
    def search_libs(self):
        with open(self.elf_path, 'rb') as elf_file:
            elf_content = elf_file.read()
            crate_matches = re.findall(b'/.cargo/(.+?)\.rs', elf_content)
            crate_matches.extend(re.findall(b'/cargo/(.+?)\.rs', elf_content))
            for crate_match in crate_matches:
                if b'\x00' in crate_match:
                    crate_match = crate_match[:crate_match.find(b'\x00')]
                crate = crate_match.split(b'/')[3].decode()
                crate_name = crate[:crate.rfind('-')]
                crate_version = crate[len(crate_name)+1:]
                if crate_name not in self.libs:
                    self.libs[crate_name] = crate_version
            self.libs = dict(sorted(self.libs.items()))
        elf_file.close()
        
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
                
    def download_and_build_libs(self, session_dir):
        logging.info('Downloading '+str(len(self.libs))+' crate(s)...')
        for crate_name in self.libs:
            crate_version = self.libs[crate_name]
            crate_url = 'https://crates.io/api/v1/crates/'+crate_name+'/'+crate_version+'/download'
            req=requests.get(crate_url)
            rsp=req.content
            file=open(session_dir+"/"+crate_name,"wb")
            file.write(rsp)
            file.close()
            # urllib.request.urlretrieve(crate_url, session_dir+'/'+crate_name)
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
            logging.success('Done ! ('+str(built_crates_ctr)+'/'+str(len(self.libs))+')')
            return True
        else:
            logging.fatal('Error when using cargo command !')
        return False

    def get_lib_path(self, lib_dir):
        if self.elf_arch == ELF.ARCH.i386:
            return lib_dir+'/target/i686-unknown-linux-gnu/release'
        return lib_dir+'/target/release'

    def should_analyze_with_r2(self):
        return False

