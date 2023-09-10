#include <command/command_executor.h>
#include <unistd.h>
#include <cstring>

using namespace std;

void CommandExecutor::execute_command(string command, COMMAND_RESULT* result) {
    if (chdir(this->env_dir.c_str()) != 0) {
        result->code = -1;
        result->response = "";
        return;
    }
    FILE* pipe = popen((command+string(" 2>&1")).c_str(), "r");
    if (!pipe) {
        result->code = -1;
        result->response = "";
        return;
    }
    stringstream ss;
    char buffer[128];
    while (fgets(buffer, sizeof(buffer), pipe) != nullptr) ss << string(buffer, strlen(buffer));
    result->code = pclose(pipe);
    result->response = ss.str();
    return;
}