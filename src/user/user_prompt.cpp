#include <user/user_prompt.h>
#include <utils/logger.h>
#include <utils/convert.h>
#include <iostream>

using namespace std;

bool ask_yes_no(string question, bool should_yes) {
    fcout << "$(info)" << question << " (" << (should_yes?"Y":"y") << "/" << (should_yes?"n":"N") << ") ";
    string response;
    getline(cin, response);
    if(!question.size()) return should_yes;
    if(should_yes) {
        if(tolower(response.at(0), locale()) == 'n') return false;
        return true;
    } else {
        if(tolower(response.at(0), locale()) == 'y') return true;
        return false;
    }
}

uint8_t ask_n(string question, uint8_t min, uint8_t max) {
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