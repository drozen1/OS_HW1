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
    cout<<"smash: got ctrl-Z"<<std::endl;
    if(is_there_a_process){
            kill( pid_to_stop, SIGSTOP);
            cout<<"smash: process "<<pid_to_stop<<" was stopped"<<std::endl;
            smash.set_there_is_a_process_running_in_the_front(false);
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
    cout<<"smash: got ctrl-C"<<std::endl;
    if(is_there_a_process){
        kill( pid_to_stop, SIGINT);
        cout<<"smash: process "<<pid_to_stop<<" was killed"<<std::endl;
    }
}

void alarmHandler(int sig_num) {
    if(sig_num == SIGALRM){
        SmallShell& smash = SmallShell::getInstance();
        cout<< "smash: got an alarm"<<endl;
        for (vector<JobEntry>::iterator it = smash.gettimeout_list().getVector().begin();
             it != smash.gettimeout_list().getVector().end(); ++it) {
            time_t timer = time(NULL);
            time_t seconds = difftime(timer,it->getLast_start_time());
            if (seconds == it->getRunning_time()){
//                if (!it->finishedTimeout){
                    int pid = it->getpid();
                    cout<< "smash: ";
                    it->print_cmd_line();
                    cout<< " timed out!" <<endl;
                    //timeout with pipe wont be tested, we can use kill
                    int res = kill(pid, SIGKILL);
                    if(res == -1){
                        perror("smash error: kill failed");
                    }
//                }
                it = smash.gettimeout_list().getVector().erase(it);
            }else{
                it++;
            }
        }
        smash.set_alarm();
    }
}
