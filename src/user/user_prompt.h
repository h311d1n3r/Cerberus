#ifndef CERBERUS_USER_PROMPT_H
#define CERBERUS_USER_PROMPT_H

#include <string>
#include <cstdint>

extern bool NO_PROMPT;

bool ask_yes_no(std::string question, bool should_yes);
uint8_t ask_n(std::string question, uint8_t min, uint8_t max);
std::string ask_password(std::string question);

#endif //CERBERUS_USER_PROMPT_H
