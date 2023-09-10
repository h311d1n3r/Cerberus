#ifndef CERBERUS_COMMAND_EXECUTOR_H
#define CERBERUS_COMMAND_EXECUTOR_H

#include <cstdint>
#include <string>
#include <filesystem>

struct COMMAND_RESULT {
    int32_t code;
    std::string response;
};

class CommandExecutor {
private:
    std::string env_dir;
public:
    CommandExecutor(std::string env_dir) : env_dir(std::filesystem::absolute(env_dir)) {};
    void execute_command(std::string command, COMMAND_RESULT* result);
};

#endif //CERBERUS_COMMAND_EXECUTOR_H
