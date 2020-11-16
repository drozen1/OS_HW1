#include <iostream>
#include <unistd.h>
#include <sys/wait.h>
#include <signal.h>
#include "Commands.h"
#include "signals.h"

int main(int argc, char *argv[]) {
//    if(signal(SIGTSTP , ctrlZHandler)==SIG_ERR) {
//        perror("smash error: failed to set ctrl-Z handler");
//    }
//    if(signal(SIGINT , ctrlCHandler)==SIG_ERR) {
//        perror("smash error: failed to set ctrl-C handler");
//    }

    //TODO: setup sig alarm handler
    ChpromptCommand call = ChpromptCommand(nullptr, 0);
    SmallShell &smash = SmallShell::getInstance();
     char* temp = getcwd(NULL, 0);
    char* arg_to_cd[0];
    arg_to_cd[0]=temp;
    ChangeDirCommand it_is_cd=ChangeDirCommand(arg_to_cd, 2);
    while (true) {
        //print prompt
        call.execute();

        std::string cmd_line;
        std::getline(std::cin, cmd_line);
        smash.executeCommand(cmd_line.c_str(), call,it_is_cd);
    }
    return 0;
}