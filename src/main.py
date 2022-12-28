import logging
import os
import sys
import shutil
from uuid import uuid4
from elf_handler import ELFHandler
from log import LogFormatter

def init_logging():
    fmt = LogFormatter()
    hdlr = logging.StreamHandler(sys.stdout)
    hdlr.setFormatter(fmt)
    logging.root.addHandler(hdlr)
    logging.root.setLevel(logging.DEBUG)

def print_help_message():
    pass

def manage_crates(elf_handler):
    if len(elf_handler.crates) > 0:
            logging.info('The following crates were found :')
            for crate_name in elf_handler.crates:
                crate_version = elf_handler.crates[crate_name]
                logging.success('- '+crate_name+': '+'\033[0;'+str(LogFormatter.LOG_COLORS['BRIGHT_GREEN'])+'m'+
                    'v'+crate_version)
    else:
        logging.warning('No crate was found in specified ELF file')
    while True:
        usr_more_crates = input('\033[0;'+str(LogFormatter.FORMAT_COLORS[logging.INFO])+'m'+
            LogFormatter.FORMAT_PREFIXES[logging.INFO]+'Add/Edit crate ? (y/N): ').strip()
        if not usr_more_crates.lower().startswith('y'):
            break
        usr_crate_name = input('\033[0;'+str(LogFormatter.FORMAT_COLORS[logging.INFO])+'m'+
            LogFormatter.FORMAT_PREFIXES[logging.INFO]+'Crate name: ').strip()
        usr_crate_version = input('\033[0;'+str(LogFormatter.FORMAT_COLORS[logging.INFO])+'m'+
            LogFormatter.FORMAT_PREFIXES[logging.INFO]+'Crate version: ').strip()
        if usr_crate_version.startswith('v'):
            usr_crate_version = usr_crate_version[1:]
        elf_handler.crates[usr_crate_name] = usr_crate_version
        logging.info('Current crates list :')
        for crate_name in elf_handler.crates:
            crate_version = elf_handler.crates[crate_name]
            logging.success('- '+crate_name+': '+'\033[0;'+str(LogFormatter.LOG_COLORS['BRIGHT_GREEN'])+'m'+
                'v'+crate_version)
    if len(elf_handler.crates) >= 1:
        return True
    return False

if __name__ == '__main__':
    init_logging()
    if len(sys.argv) >= 2:
        elf_handler = ELFHandler(sys.argv[1])
        if manage_crates(elf_handler):
            session_dir = '.cerberus-' + uuid4().hex
            while os.path.exists(session_dir):
                session_dir = '.cerberus-' + uuid4().hex
            os.mkdir(session_dir)
            if elf_handler.download_and_build_crates(session_dir):
                elf_handler.gen_hashes(session_dir)
                elf_handler.compare_hashes(session_dir)
                elf_handler.patch_elf(sys.argv[1]+'-patched')
            shutil.rmtree(session_dir)
    else:
        print_help_message()
