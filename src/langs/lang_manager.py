from enum import Enum
import re

class LANG(Enum):
    Rust = 'Rust'
    Go = 'Go'

class LangIdentifier:

    LANG_PATTERNS = {
        b'/rustc-': LANG.Rust,
        b'/.cargo/': LANG.Rust,
        b'/go-': LANG.Go,
        b'runtime.go': LANG.Go
    }

    def __init__(self, elf_path):
        self.elf_path = elf_path
        
    def identify(self):
        found_langs = {}
        with open(self.elf_path, 'rb') as elf_file:
            elf_content = elf_file.read()
            for pattern in self.LANG_PATTERNS:
                matches = re.findall(pattern, elf_content)
                if len(matches) == 0:
                    continue
                if self.LANG_PATTERNS[pattern] not in found_langs:
                    found_langs[self.LANG_PATTERNS[pattern]] = len(matches)
                else:
                    found_langs[self.LANG_PATTERNS[pattern]] += len(matches)
        elf_file.close()
        return dict(sorted(found_langs.items(), key=lambda x: x[1], reverse=True))
        
    def lang_from_name(self, lang_name):
        for lang in LANG:
            if lang.value.lower() == lang_name.lower():
                return lang
        return None
