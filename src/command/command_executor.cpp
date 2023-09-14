#include <command/command_executor.h>
#include <unistd.h>
#include <cstring>

using namespace std;

bool CommandExecutor::test_password(std::string password) {
    COMMAND_RESULT res;
    this->execute_command("echo "+password+" | sudo -Sk echo test", &res);
    return !res.code;
}

void CommandExecutor::execute_command(string command, COMMAND_RESULT* result) {
    char current_dir[1024];
    getcwd(current_dir, sizeof(current_dir));
    if (chdir(this->env_dir.c_str()) != 0) {
        result->code = -1;
        result->response = "";
        return;
    }
    FILE* pipe = popen((command+string(" 2>&1")).c_str(), "r");
    if (!pipe) {
        result->code = -1;
        result->response = "";
        chdir(current_dir);
        return;
    }
    stringstream ss;
    char buffer[128];
    while (fgets(buffer, sizeof(buffer), pipe) != nullptr) ss << string(buffer, strlen(buffer));
    result->code = pclose(pipe);
    result->response = ss.str();
    chdir(current_dir);
}