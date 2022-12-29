import logging

logging.SUCCESS = 25
logging.addLevelName(logging.SUCCESS, 'SUCCESS')
setattr(logging, 'success', lambda message, *args: logging.root._log(logging.SUCCESS, message, args))

class LogFormatter(logging.Formatter):
    
    LOG_COLORS = {
        'BLACK': 30,
        'RED': 31,
        'GREEN': 32,
        'YELLOW': 33,
        'BLUE': 34,
        'MAGENTA': 35,
        'CYAN': 36,
        'WHITE': 37,
        'BRIGHT_BLACK': 90,
        'BRIGHT_RED': 91,
        'BRIGHT_GREEN': 92,
        'BRIGHT_YELLOW': 93,
        'BRIGHT_BLUE': 94,
        'BRIGHT_MAGENTA': 95,
        'BRIGHT_CYAN': 96,
        'BRIGHT_WHITE': 97
    }
    
    FORMAT_COLORS = {
        logging.DEBUG: LOG_COLORS['BRIGHT_BLUE'],
        logging.SUCCESS: LOG_COLORS['GREEN'],
        logging.INFO: LOG_COLORS['CYAN'],
        logging.WARNING: LOG_COLORS['YELLOW'],
        logging.ERROR: LOG_COLORS['MAGENTA'],
        logging.CRITICAL: LOG_COLORS['RED']
    }
    
    FORMAT_PREFIXES = {
        logging.DEBUG: '[-] ',
        logging.SUCCESS: '[+] ',
        logging.INFO: '[*] ',
        logging.WARNING: '[#] ',
        logging.ERROR: '[?] ',
        logging.CRITICAL: '[!] '
    }
    
    def __init__(self, fmt='%(message)s'):
        logging.Formatter.__init__(self, fmt)
        
    def format(self, record):
        base_format = self._fmt
        format_color = 37
        if record.levelno in self.FORMAT_COLORS:
            format_color = self.FORMAT_COLORS[record.levelno]
        format_prefix = ''
        if record.levelno in self.FORMAT_PREFIXES:
            format_prefix = self.FORMAT_PREFIXES[record.levelno]
        self._fmt = '\033[0;'+str(format_color)+'m'+format_prefix+'%(message)s'
        result = logging.Formatter(self._fmt).format(record)
        self._fmt = base_format
        return result
