#include <utils/logger.h>
#include <iostream>

using namespace std;

FCout fcout;

map<string, uint8_t> LOG_COLORS = {
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

map<string, pair<uint8_t, uint8_t>> LOG_LEVELS = {
    {"debug", pair(94, '-')},
    {"success", pair(32, '+')},
    {"info", pair(36, '*')},
    {"warning", pair(33, '#')},
    {"error", pair(35, '?')},
    {"critical", pair(31, '!')}
};

map<char, uint8_t> LOG_STYLES = {
    {'n', 0},
    {'b', 1},
    {'i', 3},
    {'u', 4},
    {'s', 9},
};

FCout FCout::operator<<(string s) {
    format(s);
    std::cout << s;
    return *this;
}

FCout FCout::operator<<(std::ostream&(*pManip)(std::ostream&)) {
    std::cout << *pManip << "\033[0;"+to_string(LOG_COLORS["gray"])+"m";
    args_stack.clear();
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
                uint8_t font_mode_chr = arg_name.at(index2+1);
                if(LOG_STYLES.find(font_mode_chr) != LOG_STYLES.end()) {
                    font_mode = LOG_STYLES[font_mode_chr];
                }
                arg_name = arg_name.substr(0, arg_name.length()-2);
            }
            if(LOG_COLORS.find(arg_name) != LOG_COLORS.end()) {
                args_stack.push_back(pair(LOG_COLORS[arg_name], font_mode));
                res += "\033["+to_string(font_mode)+";"+to_string(LOG_COLORS[arg_name])+"m";
            } else if(LOG_LEVELS.find(arg_name) != LOG_LEVELS.end()) {
                pair<uint8_t, uint8_t> level = LOG_LEVELS[arg_name];
                res += "\033["+to_string(font_mode)+";"+to_string(level.first)+"m";
                if(this->args_stack.size() == 0 || this->args_stack.at(this->args_stack.size()-1).first != level.first) res += string("[")+(char)level.second+"] ";
                args_stack.push_back(pair(level.first, font_mode));
            }
            s = s.substr(index+3+arg_name.length()+(has_font_mode?2:0));
        } else if(args_stack.size() > 0) {
            args_stack.pop_back();
            pair<uint8_t, uint8_t> arg;
            if(args_stack.size() > 0) arg = args_stack.at(args_stack.size()-1);
            else arg = pair(LOG_COLORS["gray"], 0);
            res += "\033["+to_string(arg.second)+";"+to_string(arg.first)+"m";
            s = s.substr(index+1);
        }
   }
   res += s;
   s = res;
}