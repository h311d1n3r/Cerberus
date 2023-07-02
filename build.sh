#!/bin/bash

pyinstaller $(python3 -c 'import glob; import os; print("--hidden-import langs."+".elf_handler --hidden-import langs.".join([x[len("src/langs/"):-1] for x in glob.glob("src/langs/*/", recursive=True)])+".elf_handler")') src/cerberus.py $(python3 -c 'import glob; import os; print(" ".join(glob.glob("src/**/*.py", recursive=True)))') --onefile --clean
