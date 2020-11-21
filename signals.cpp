#include <iostream>
#include <signal.h>
#include "signals.h"
#include "Commands.h"

using namespace std;

void ctrlZHandler(int sig_num) {
    SmallShell &smash = SmallShell::getInstance();
    JobsList jobsList= smash.getJobList();
    bool is_there_a_process=smash.getBool();
    ForegroundCommand* frontcmd=smash.getFrondCmd();
    cout<<"smash: got ctrl-Z"<<"\n";
    if(is_there_a_process){
        pid_t curr_pid = frontcmd->getpid();
        JobsList::JobEntry* job_entry=jobsList.getJobByPid(curr_pid);
        if(job_entry== nullptr) {
            jobsList.addJob(frontcmd, frontcmd->getpid(), false);
            job_entry=jobsList.getJobByPid(curr_pid);
        }

            jobsList.killCommand(job_entry->getJob_id(), SIGSTOP);
    }
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
    // TODO: Add your implementation
}

void alarmHandler(int sig_num) {
    // TODO: Add your implementation
}
