import re
import logging
import urllib.request
import os
import gzip
import shutil
import tarfile
import subprocess
import requests
import sys
import params
from shutil import which
from lief import ELF
from log import LogFormatter
from build_fixer import BuildFixer
from abs_elf_handler import AbstractELFHandler
import xml.etree.ElementTree as ET
from bs4 import BeautifulSoup

class ELFHandler(AbstractELFHandler):
    
    def __init__(self, elf_path):
        super().__init__(elf_path)
        
    def search_libs(self):
        self.go_version = None
        with open(self.elf_path, 'rb') as elf_file:
            elf_content = elf_file.read()
            self.go_version = re.findall(b'go(\d)\.(\d{2})?(\.\d)?', elf_content)
            if self.go_version is None:
                logging.fatal('Unable to identify Go version...')
                sys.exit(1)
            self.go_version = self.go_version[0]
            self.go_version = self.go_version[0].decode() + '.' + self.go_version[1].decode() + self.go_version[2].decode()
            logging.info('Identified \033[1;'+str(LogFormatter.LOG_COLORS['MAGENTA'])+'mGo v'+self.go_version)
            lib_matches = re.findall(b'go(.*?)/pkg/mod/(.+?)\.(s|go)', elf_content)
            for lib_match in lib_matches:
                if lib_match[2] == b's':
                    continue
                lib_match = lib_match[1]
                if b'golang.org' in lib_match:
                    continue
                lib_name = lib_match[:lib_match.rindex(b'@')].decode()
                lib_version = lib_match[lib_match.rindex(b'@')+2:]
                lib_version = lib_version[:lib_version.index(b'/')].decode()
                if lib_name not in self.libs:
                    self.libs[lib_name] = lib_version
            lib_matches = re.findall(b'go(.*?)/src/(.+?)\.(s|go)', elf_content)
            for lib_match in lib_matches:
                if lib_match[2] == b's':
                    continue
                lib_match = lib_match[1]
                if lib_match[:len(b'go/src/')] == b'go/src/':
                    lib_match = lib_match[len(b'go/src/'):]
                if b'internal' in lib_match or b'runtime' in lib_match or b'github.com/':
                    continue
                if b'/' in lib_match:
                    lib_match = lib_match[:lib_match.rindex(b'/')]
                lib_name = 'std::'+lib_match.decode()
                if lib_name not in self.libs:
                    self.libs[lib_name] = '.unk'
            self.libs = dict(sorted(self.libs.items()))
        elf_file.close()

    def check_goliath_installation(self):
        logging.info('Checking for \033[1;'+str(LogFormatter.FORMAT_COLORS[logging.INFO])+'mGoliath\033[0;'+str(LogFormatter.FORMAT_COLORS[logging.INFO])+'m installation...')
        goliath_dir = os.path.expanduser("~")+'/.local/bin/'
        if goliath_dir not in sys.path:
            sys.path.append(goliath_dir)
        if which('goliath') is not None:
            logging.success('\033[1;'+str(LogFormatter.FORMAT_COLORS[logging.SUCCESS])+'mGoliath\033[0;'+str(LogFormatter.FORMAT_COLORS[logging.SUCCESS])+'m was found !')
            return True
        logging.warning('Couldn\'t find \033[1;'+str(LogFormatter.FORMAT_COLORS[logging.WARNING])+'mGoliath\033[0;'+str(LogFormatter.FORMAT_COLORS[logging.WARNING])+'m on PATH !')
        logging.info('\033[1;'+str(LogFormatter.FORMAT_COLORS[logging.INFO])+'mGoliath\033[0;'+str(LogFormatter.FORMAT_COLORS[logging.INFO])+'m is a fork of the go compiler that prevents deadcode elimination (https://github.com/h311d1n3r/Goliath).')
        if not params.NO_PROMPT:
            usr_dl_goliath = input('\033[0;'+str(LogFormatter.FORMAT_COLORS[logging.INFO])+'mDownload latest version ? (Y/n) : \033[0;'+str(LogFormatter.LOG_COLORS['WHITE'])+'m')
            if usr_dl_goliath.lower().startswith('n'):
                return False
        release_url = 'https://github.com/h311d1n3r/Goliath/releases.atom'
        response = requests.get(release_url)
        if response.status_code == 200:
            root = ET.fromstring(response.content)
            ns = {'ns': 'http://www.w3.org/2005/Atom'}
            #latest one
            entry = root.find('ns:entry', ns)
            title = entry.find('ns:title', ns).text
            link = entry.find('ns:link', ns).attrib['href']
            download_url = 'https://github.com/h311d1n3r/Goliath/releases/download/'+link[link.rindex('/')+1:]+'/goliath'
            logging.info('Downloading...')
            response = requests.get(download_url)
            if response.status_code == 200:
                os.makedirs(goliath_dir, exist_ok=True)
                with open(goliath_dir + 'goliath', 'wb') as save_file:
                    save_file.write(response.content)
                    save_file.close()
                os.chmod(goliath_dir + 'goliath', 0o777)
                logging.success('Done !')
                return True
            else:
                logging.error('An error occured when attempting to download \033[1;'+str(LogFormatter.FORMAT_COLORS[logging.ERROR])+'mGoliath\033[0;'+str(LogFormatter.FORMAT_COLORS[logging.ERROR])+'m...')
        else:
            logging.error('An error occured when attempting to find download link of \033[1;'+str(LogFormatter.FORMAT_COLORS[logging.ERROR])+'mGoliath\033[0;'+str(LogFormatter.FORMAT_COLORS[logging.ERROR])+'m...')
        return False

    def build_go_version(self, session_dir):
        logging.info('Starting \033[1;'+str(LogFormatter.FORMAT_COLORS[logging.INFO])+'mGoliath\033[0;'+str(LogFormatter.FORMAT_COLORS[logging.INFO])+'m to patch and build Go v'+self.go_version+' !')
        print('\033[1;'+str(LogFormatter.LOG_COLORS['MAGENTA'])+'m------- GOLIATH -------')
        goliath_proc = subprocess.Popen(['goliath', self.go_version], cwd=session_dir)
        goliath_proc.communicate()
        print('\033[1;'+str(LogFormatter.LOG_COLORS['MAGENTA'])+'m-----------------------')
        if goliath_proc.returncode == 1:
            return False
        return True

    def download_and_build_libs(self, session_dir):
        os.mkdir(session_dir+'/standalone')
        if not self.check_goliath_installation():
            return False
        if not self.build_go_version(session_dir):
            return False
        go_path = 'go-'+self.go_version+'/go/bin/go'
        logging.info('Preparing build module...')
        init_output = subprocess.run([go_path, 'mod', 'init', 'build_mod'], cwd=session_dir, capture_output=True)
        if b'creating new go.mod' not in init_output.stderr:
            logging.fatal('Couldn\'t build module...')
            logging.fatal(init_output.stderr.decode())
            return False
        with open(session_dir+'/build_mod.go', 'w') as program_file:
            program_file.write('package main\n\n')
            program_file.write('import (\n')
            for lib_name in self.libs:
                if lib_name[:len('std::')] == 'std::':
                    lib_name = lib_name[len('std::'):]
                program_file.write('\t_ \"'+lib_name+'\"\n')
            program_file.write(')\n\n')
            program_file.write('func main() {}')
            program_file.close()
        logging.info('Downloading libraries...')
        for lib_name in self.libs:
            if lib_name[:len('std::')] == 'std::':
                continue
            lib_version = self.libs[lib_name]
            download_output = subprocess.run([go_path, 'get', lib_name+'@v'+lib_version], cwd=session_dir, capture_output=True)
            if download_output.returncode != 0:
                logging.error('Couldn\'t download '+lib_name+' with version v'+lib_version+'.')
                logging.error(download_output.stderr.decode())
        logging.info('Building standalone ELF with \033[1;'+str(LogFormatter.FORMAT_COLORS[logging.INFO])+'mgoliath\033[0;'+str(LogFormatter.FORMAT_COLORS[logging.INFO])+'m...')
        build_output = subprocess.run([go_path, 'build', '-o', 'standalone/standalone.so', 'build_mod.go'], cwd=session_dir, capture_output=True)
        if build_output.returncode != 0:
            logging.error('An error occured during building process.')
            logging.error(build_output.stderr.decode())
            return False
        os.remove(session_dir+'/go.mod')
        if os.path.exists(session_dir+'/go.sum'):
            os.remove(session_dir+'/go.sum')
        os.remove(session_dir+'/build_mod.go')
        shutil.rmtree(session_dir+'/go-'+self.go_version)
        logging.success('Done !')
        return True

    def get_lib_path(self, lib_dir):
        return lib_dir
