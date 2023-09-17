#include <user/user_prompt.h>
#include <utils/logger.h>
#include <utils/convert.h>
#include <iostream>
#include <termios.h>
#include <unistd.h>

using namespace std;

bool NO_PROMPT = false;

bool ask_yes_no(string question, bool should_yes) {
    if(NO_PROMPT) return should_yes;
    fcout << "$(info)" << question << " (" << (should_yes?"Y":"y") << "/" << (should_yes?"n":"N") << ") $";
    string response;
    getline(cin, response);
    if(!response.size()) return should_yes;
    if(should_yes) {
        if(tolower(response.at(0), locale()) == 'n') return false;
        return true;
    } else {
        if(tolower(response.at(0), locale()) == 'y') return true;
        return false;
    }
}

uint8_t ask_n(string question, uint8_t min, uint8_t max) {
    if(NO_PROMPT) return min;
    fcout << "$(info)" << question << " (" << to_string(min) << "-" << to_string(max) << ") $";
    string response;
    uint8_t response_i = max + 1;
    while(response_i < min || response_i > max) {
        getline(cin, response);
        while (!string_to_int<uint8_t>(response, response_i)) {
            fcout << "$(error)Wrong value, please try again (" << to_string(min) << "-" << to_string(max) << ") $";
            getline(cin, response);
        }
        if(response_i < min || response_i > max) fcout << "$(error)Value is not in range, please try again (" << to_string(min) << "-" << to_string(max) << ") $";
    }
    return response_i;
}

string ask_password(string question) {
    termios oldt;
    tcgetattr(STDIN_FILENO, &oldt);
    termios newt = oldt;
    newt.c_lflag &= ~ECHO;
    tcsetattr(STDIN_FILENO, TCSANOW, &newt);
    fcout << "$(info)" << question << ": $";
    string password;
    getline(cin, password);
    fcout << endl;
    tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
    return password;
}