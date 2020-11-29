#include <iostream>
#include <unistd.h>
#include <sys/wait.h>
#include <signal.h>
#include "Commands.h"
#include "signals.h"

int main(int argc, char *argv[]) {

    SmallShell &smash = SmallShell::getInstance();
    smash.shell_pid=getpid();
    //JobsList jobsList=smash.getJobList().killCommand();
    if(signal(SIGTSTP , ctrlZHandler)==SIG_ERR) {
        perror("smash error: failed to set ctrl-Z handler");
    }
    if(signal(SIGINT , ctrlCHandler)==SIG_ERR) {
        perror("smash error: failed to set ctrl-C handler");
    }

    struct sigaction struct_of_sigaction;
    struct_of_sigaction.sa_handler = alarmHandler;
    sigemptyset(&struct_of_sigaction.sa_mask);
    struct_of_sigaction.sa_flags = SA_RESTART;
    if (sigaction(SIGALRM, &struct_of_sigaction, NULL) == -1){
        perror("smash error: failed to set alarm handler");
    }

    //TODO: setup sig alarm handler
    ChpromptCommand call = ChpromptCommand(nullptr, 0);

//    char* arg_to_cd[COMMAND_MAX_ARGS];
//    char command[]="cd";
//    arg_to_cd[0]=command;
//    arg_to_cd[1]=NULL;
    ChangeDirCommand it_is_cd=ChangeDirCommand(NULL, 0);
    while (true) {
        //print prompt
        call.execute();
        std::string cmd_line;
        std::getline(std::cin, cmd_line);
        smash.executeCommand(cmd_line.c_str(), call,it_is_cd);
    }
    return 0;
}
