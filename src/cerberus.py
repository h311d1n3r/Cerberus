import logging
import os
import sys
import shutil
import argparse
import params
from uuid import uuid4
from elf_handler import ELFHandler
from log import LogFormatter

TOOL_TITLE = "   ___         _       \n\
  / __|___ _ _| |__  ___ _ _ _  _ ___\n\
 | (__/ -_) '_| '_ \/ -_) '_| || (_-<\n\
  \___\___|_| |_.__/\___|_|  \_,_/__/\n"
VERSION = '1.1'
AUTHOR = 'h311d1n3r'

def init_logging():
    fmt = LogFormatter()
    hdlr = logging.StreamHandler(sys.stdout)
    hdlr.setFormatter(fmt)
    logging.root.addHandler(hdlr)
    if params.DEBUG:
        logging.root.setLevel(logging.DEBUG)
    else:
        logging.root.setLevel(logging.INFO)

def print_help_message():
    print('\033[0;'+str(LogFormatter.LOG_COLORS['CYAN'])+'m'+TOOL_TITLE)
    print('\033[0;'+str(LogFormatter.LOG_COLORS['CYAN'])+'m Version: \033[1;'+
        str(LogFormatter.LOG_COLORS['WHITE'])+'m'+VERSION)
    print('\033[0;'+str(LogFormatter.LOG_COLORS['CYAN'])+'m Author: \033[1;'+
        str(LogFormatter.LOG_COLORS['WHITE'])+'m'+AUTHOR)
    print('\033[0;'+str(LogFormatter.LOG_COLORS['CYAN'])+'m_____________________________________\n')
    print('\033[0;'+str(LogFormatter.LOG_COLORS['CYAN'])+'mSyntax: \033[1;'+
        str(LogFormatter.LOG_COLORS['YELLOW'])+'mcerberus binary [-param value] [--flag]\n')
    print('\033[0;'+str(LogFormatter.LOG_COLORS['CYAN'])+'mParameters:')
    print('\033[1;'+str(LogFormatter.LOG_COLORS['YELLOW'])+'m   output\033[0;'+str(LogFormatter.LOG_COLORS['CYAN'])+'m'+
        ' -> Specifies the path for the resulting ELF file. \033[1;'+str(LogFormatter.LOG_COLORS['CYAN'])+'m'+'Default value : [input_binary]-patched')
    print('\033[1;'+str(LogFormatter.LOG_COLORS['YELLOW'])+'m   part_hash_len\033[0;'+str(LogFormatter.LOG_COLORS['CYAN'])+'m'+
' -> Specifies the length of a part hash. The part hash of a function is just a reduction of the function with a linear pace.\
 This technique is used to prevent fixed addresses from corrupting a standard hash. \033[1;'+str(LogFormatter.LOG_COLORS['CYAN'])+'m'+'Default value : 20')
    print('\033[1;'+str(LogFormatter.LOG_COLORS['YELLOW'])+'m   part_hash_trust\033[0;'+str(LogFormatter.LOG_COLORS['CYAN'])+'m'+
' -> Specifies minimum ratio of similarity between the two hashed functions to compare. The kept function will be the one with the most matches anyway.\
 Increasing this value will reduce the number of matched functions but speed up execution time. \033[1;'+str(LogFormatter.LOG_COLORS['CYAN'])+'m'+'Default value : 0.6')
    print('\033[1;'+str(LogFormatter.LOG_COLORS['YELLOW'])+'m   min_func_size\033[0;'+str(LogFormatter.LOG_COLORS['CYAN'])+'m'+
        ' -> Specifies the minimum length a function must be to get analyzed. Decreasing this value will increase matches but also false positives. \033[1;'+
        str(LogFormatter.LOG_COLORS['CYAN'])+'m'+'Default value : 10')
    print('\033[0;'+str(LogFormatter.LOG_COLORS['CYAN'])+'m\nFlags:')
    print('\033[1;'+str(LogFormatter.LOG_COLORS['YELLOW'])+'m   help\033[0;'+str(LogFormatter.LOG_COLORS['CYAN'])+'m'+
        ' -> Displays this message.')
    print('\033[1;'+str(LogFormatter.LOG_COLORS['YELLOW'])+'m   debug\033[0;'+str(LogFormatter.LOG_COLORS['CYAN'])+'m'+
        ' -> Enable debug level of logging.')

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
            LogFormatter.FORMAT_PREFIXES[logging.INFO]+'Add/Edit/Remove crate ? (y/N): ').strip()
        if not usr_more_crates.lower().startswith('y'):
            break
        usr_crate_name = input('\033[0;'+str(LogFormatter.FORMAT_COLORS[logging.INFO])+'m'+
            LogFormatter.FORMAT_PREFIXES[logging.INFO]+'Crate name: ').strip()
        usr_crate_version = input('\033[0;'+str(LogFormatter.FORMAT_COLORS[logging.INFO])+'m'+
            LogFormatter.FORMAT_PREFIXES[logging.INFO]+'Crate version (blank to remove): ').strip()
        if usr_crate_name in elf_handler.crates and len(usr_crate_version) == 0:
            del elf_handler.crates[usr_crate_name]
        else:
            if usr_crate_version.startswith('v'):
                usr_crate_version = usr_crate_version[1:]
            elf_handler.crates[usr_crate_name] = usr_crate_version
        logging.info('Current crates list :')
        for crate_name in elf_handler.crates:
            crate_version = elf_handler.crates[crate_name]
            logging.success('- '+crate_name+': \033[0;'+str(LogFormatter.LOG_COLORS['BRIGHT_GREEN'])+'m'+
                'v'+crate_version)
    if len(elf_handler.crates) >= 1:
        return True
    return False

if __name__ == '__main__':
    parser = argparse.ArgumentParser(add_help=False)
    parser.add_argument('binary', nargs='?', type=str)
    parser.add_argument('-output', dest='output', type=str)
    parser.add_argument('-part_hash_trust', dest='part_hash_trust', type=float)
    parser.add_argument('-part_hash_len', dest='part_hash_len', type=int)
    parser.add_argument('-min_func_size', dest='min_func_size', type=int)
    parser.add_argument('--help', action='store_true')
    parser.add_argument('--debug', action='store_true')
    args = parser.parse_args()
    params.DEBUG = args.debug
    init_logging()
    if args.part_hash_trust:
        params.PART_HASH_TRUST = args.part_hash_trust
    if args.part_hash_len:
        params.PART_HASH_LEN = args.part_hash_len
    if args.min_func_size:
        params.MIN_FUNC_SIZE = args.min_func_size
    if args.help:
        print_help_message()
        sys.exit(0)
    if args.binary:
        params.OUTPUT = args.binary+'-patched'
        if args.output:
            params.OUTPUT = args.output
        elf_handler = ELFHandler(args.binary)
        if manage_crates(elf_handler):
            session_dir = '.cerberus-' + uuid4().hex
            while os.path.exists(session_dir):
                session_dir = '.cerberus-' + uuid4().hex
            os.mkdir(session_dir)
            if elf_handler.download_and_build_crates(session_dir):
                elf_handler.gen_hashes(session_dir)
                elf_handler.compare_hashes(session_dir)
                elf_handler.patch_elf()
                logging.success('End of execution. ELF file \033[0;'+str(LogFormatter.LOG_COLORS['WHITE'])+'m'+
                    params.OUTPUT+'\033[0;'+str(LogFormatter.FORMAT_COLORS[logging.SUCCESS])+'m is your result.')
            shutil.rmtree(session_dir)
    else:
        print_help_message()
