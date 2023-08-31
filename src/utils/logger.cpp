#include "logger.h"
#include <iostream>

using namespace std;

FCout fcout;
std::map<std::string, uint8_t> LOG_COLORS = {
    {"black", 30},
    {"red", 31},
    {"green", 32},
    {"yellow", 33},
    {"blue", 34},
    {"magenta", 35},
    {"cyan", 36},
    {"white", 37},
    {"bright_black", 90},
    {"bright_red", 91},
    {"bright_green", 92},
    {"bright_yellow", 93},
    {"bright_blue", 94},
    {"bright_magenta", 95},
    {"bright_cyan", 96},
    {"bright_white", 97}
};

FCout FCout::operator<<(string s) {
    format(s);
    std::cout << s;
    return *this;
}

FCout FCout::operator<<(std::ostream&(*pManip)(std::ostream&)) {
    std::cout << *pManip;
    return *this;
}

void FCout::format(std::string& s) {
   size_t index;
   string res = "";
   while((index = s.find('$')) != string::npos) {
        res += s.substr(0, index);
        if(index < s.length()-1 && s.at(index+1) == '(') {
            string arg_name = s.substr(index+2);
            arg_name = arg_name.substr(0, arg_name.find(')'));
            int index2;
            uint8_t font_mode = 0;
            bool has_font_mode = false;
            if((index2 = arg_name.find(":")) != string::npos) {
                has_font_mode = true;
                if(arg_name.at(index2+1) == 'b') font_mode = 1;
                arg_name = arg_name.substr(0, arg_name.length()-2);
            }
            if(LOG_COLORS.find(arg_name) != LOG_COLORS.end()) {
                res += "\033["+to_string(font_mode)+";"+to_string(LOG_COLORS[arg_name])+"m";
            }
            s = s.substr(index+3+arg_name.length()+(has_font_mode?2:0));
        }
   }
   res += s;
   s = res;
}