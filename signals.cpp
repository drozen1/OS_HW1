#include <iostream>
#include <signal.h>
#include "signals.h"
#include "Commands.h"
#include <sys/signalfd.h>

using namespace std;

void ctrlZHandler(int sig_num) {
    SmallShell &smash = SmallShell::getInstance();
    bool is_there_a_process=smash.getBool();
    pid_t pid_to_stop=smash.getFront_cmd_pid();
    cout<<"smash: got ctrl-Z"<<"\n";
    if(is_there_a_process){
            kill( pid_to_stop, SIGSTOP);
            cout<<"smash: process "<<pid_to_stop<<" was stopped"<<"\n";
    }

    return;
}
///case: job is already in the vector
////            job_entry->SetIs_running(false);
////            double current_running_time= job_entry->getRunning_time();
////            double last_running_time= difftime(job_entry->getLast_start_time(),time(NULL));
////            job_entry->set_running_time(current_running_time+last_running_time);
//            jobsList.killCommand(job_entry->getJob_id(), SIGSTOP);
//        }else{
///case job not in the list

void ctrlCHandler(int sig_num) {
    SmallShell &smash = SmallShell::getInstance();
    bool is_there_a_process=smash.getBool();
    pid_t pid_to_stop=smash.getFront_cmd_pid();
    cout<<"smash: got ctrl-C"<<"\n";
    if(is_there_a_process){
        kill( pid_to_stop, SIGINT);
        cout<<"smash: process "<<pid_to_stop<<" was stopped"<<"\n";
    }
}

void alarmHandler(int sig_num) {
    // TODO: Add your implementation
}
