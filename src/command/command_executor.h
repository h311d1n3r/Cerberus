#ifndef CERBERUS_COMMAND_EXECUTOR_H
#define CERBERUS_COMMAND_EXECUTOR_H

#include <cstdint>
#include <string>
#include <filesystem>

extern bool COMMANDS_DEBUG_MODE;

struct COMMAND_RESULT {
    int32_t code;
    std::string response;
};

class CommandExecutor {
private:
    std::string env_dir;
public:
    CommandExecutor(std::string env_dir) : env_dir(std::filesystem::absolute(env_dir)) {};
    bool test_password(std::string password);
    void execute_command(std::string command, COMMAND_RESULT* result);
};

#endif //CERBERUS_COMMAND_EXECUTOR_H
