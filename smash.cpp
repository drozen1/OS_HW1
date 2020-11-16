#include <iostream>
#include <unistd.h>
#include <sys/wait.h>
#include <signal.h>
#include "Commands.h"
#include "signals.h"

int main(int argc, char* argv[]) {
//    if(signal(SIGTSTP , ctrlZHandler)==SIG_ERR) {
//        perror("smash error: failed to set ctrl-Z handler");
//    }
//    if(signal(SIGINT , ctrlCHandler)==SIG_ERR) {
//        perror("smash error: failed to set ctrl-C handler");
//    }

    //TODO: setup sig alarm handler
    ChpromptCommand call= ChpromptCommand(nullptr,0);
   SmallShell& smash = SmallShell::getInstance();
    while(true) {
        call.execute();
        //std::cout << "smash> "; // TODO: change this (why?)
        std::string cmd_line;
        std::getline(std::cin, cmd_line);
        smash.executeCommand(cmd_line.c_str(),call);
    }
    return 0;
}