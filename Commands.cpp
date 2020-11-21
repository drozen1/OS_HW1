#include <unistd.h>
#include <iostream>
#include <vector>
#include <sstream>
#include <sys/wait.h>
#include <iomanip>
#include "Commands.h"

using namespace std;

const std::string WHITESPACE = " \n\r\t\f\v";

#if 0
#define FUNC_ENTRY()  \
  cerr << __PRETTY_FUNCTION__ << " --> " << endl;

#define FUNC_EXIT()  \
  cerr << __PRETTY_FUNCTION__ << " <-- " << endl;
#else
#define FUNC_ENTRY()
#define FUNC_EXIT()
#endif

#define DEBUG_PRINT cerr << "DEBUG: "

#define EXEC(path, arg) \
  execvp((path), (arg));

string _ltrim(const std::string &s) {
    size_t start = s.find_first_not_of(WHITESPACE);
    return (start == std::string::npos) ? "" : s.substr(start);
}

string _rtrim(const std::string &s) {
    size_t end = s.find_last_not_of(WHITESPACE);
    return (end == std::string::npos) ? "" : s.substr(0, end + 1);
}

string _trim(const std::string &s) {
    return _rtrim(_ltrim(s));
}

int _parseCommandLine(const char *cmd_line, char **args) {
    FUNC_ENTRY()
    int i = 0;
    std::istringstream iss(_trim(string(cmd_line)).c_str());
    for (std::string s; iss >> s;) {
        args[i] = (char *) malloc(s.length() + 1);
        memset(args[i], 0, s.length() + 1);
        strcpy(args[i], s.c_str());
        args[++i] = NULL;
    }
    return i;

    FUNC_EXIT()
}

bool _isBackgroundComamnd(const char *cmd_line) {
    const string str(cmd_line);
    return str[str.find_last_not_of(WHITESPACE)] == '&';
}

void _removeBackgroundSign(char *cmd_line) {
    const string str(cmd_line);
    // find last character other than spaces
    unsigned int idx = str.find_last_not_of(WHITESPACE);
    // if all characters are spaces then return
    if (idx == string::npos) {
        return;
    }
    // if the command line does not end with & then return
    if (cmd_line[idx] != '&') {
        return;
    }
    // replace the & (background sign) with space and then remove all tailing spaces.
    cmd_line[idx] = ' ';
    // truncate the command line string up to the last non-space character
    cmd_line[str.find_last_not_of(WHITESPACE, idx) + 1] = 0;
}

// TODO: Add your implementation for classes in Commands.h 

SmallShell::SmallShell() {
    return;
// TODO: add your implementation
}

SmallShell::~SmallShell() {
    return;
// TODO: add your implementation
}

/**
* Creates and returns a pointer to Command class which matches the given command line (cmd_line)
*/
Command *SmallShell::CreateCommand(const char *cmd_line, ChpromptCommand &call, ChangeDirCommand &cd) {
    ///do to: we do a new to a command and it will be a memory lip

    char *args[COMMAND_MAX_ARGS];
    char *copy_cmd_line = const_cast<char *>(cmd_line);
    // char** check= this->my_job_list.getJobById(1)->getCommand()
    bool isBackground = false;

    if (_isBackgroundComamnd(cmd_line)) {
        //backGround
        _removeBackgroundSign(copy_cmd_line);
        isBackground = true;

    }
    there_is_a_process_running_in_the_front = false;
    const char *const_copy = copy_cmd_line;
    int len = _parseCommandLine(const_copy, args);
    char *name_of_command = args[0];
    if (len <= 1 + COMMAND_MAX_ARGS) {
        char key1[] = "chprompt";
        if (strcmp(name_of_command, key1) == 0) {
            call.changTheString(args, len);
            return nullptr;
        }
        if (len == 1) {
            char key[] = "showpid";
            if (strcmp(name_of_command, key) == 0) {
                return new ShowPidCommand(args, len);
            }

            char key2[] = "ls";
            if (strcmp(name_of_command, key2) == 0) {
                return new LsDirCommand(args, len);
            }
            char key3[] = "pwd";
            if (strcmp(name_of_command, key3) == 0) {
                return new GetCurrDirCommand(args, len);
            }
        }
        char key4[] = "cd";
        if (strcmp(name_of_command, key4) == 0) {
            cd.Set_Orig_Vals(args, len);
            cd.execute();
            return nullptr;
        }
        char key5[] = "jobs";
        if (strcmp(name_of_command, key5) == 0) {
            if (len==1) {
                this->my_job_list.printJobsList();
            }
            return nullptr;
        }
        char key6[] = "fg";
        if (strcmp(name_of_command, key6) == 0) {
            if (len > 2) {
                cout << "smash error: fg: invalid arguments\n";
                return nullptr;
            }
            if (args[1] == NULL) {
                my_job_list.fgCommand(0);
            } else {
                if (atoi(args[1]) > 0) {
                    my_job_list.fgCommand(atoi(args[1]));
                } else {
                    ///chack if to return error because we get job id<0 and not valid.

                    cout << "smash error: fg: job-id " << atoi(args[1]) << " does not exist\n";
                }
            }
            return nullptr;
        }
        char key9[] = "bg";
        if (strcmp(name_of_command, key9) == 0) {
            if (len > 2) {
                cout << "smash error: bg: invalid arguments\n";
                return nullptr;
            }
            if (args[1] == NULL) {
                my_job_list.bgCommand(0);
            } else {
                if (atoi(args[1]) > 0) {
                    my_job_list.bgCommand(atoi(args[1]));
                } else {
                    ///chack if to return error because we get job id<0 and not valid.

                    cout << "smash error: fg: job-id " << atoi(args[1]) << " does not exist\n";
                }
            }
            return nullptr;
        }
        char key7[] = "kill";
        if (strcmp(name_of_command, key7) == 0) {
            if (len == 3) {
                ////check format of signum and jobid
                char numOfSignal[2] = {args[1][1], args[1][2]};
                int signum = atoi(numOfSignal);
                //int signum = args[1][1] - '0';
                int job_id = args[2][0] - '0';
                my_job_list.killCommand(job_id, signum);
            } else {
                cout << "smash error: kill: invalid arguments\n";
            }
            return nullptr;
        }
        char key8[] = "quit";
        if (strcmp(name_of_command, key8) == 0) {
            if (len == 2) {
                char *arg1 = args[1];
                if (strcmp(arg1, key7) == 0) {  ////linoy fix the bug
                    return nullptr;
                }
            }
            if (len == 1) {
                ////what to do?
                return nullptr;
            }
            return nullptr;
        }
        if (isBackground) {

            BackgroundCommand *beckCommand = new BackgroundCommand(args, len, copy_cmd_line);
            my_job_list.addJob(beckCommand, beckCommand->getpid(), true);
            return beckCommand;

        } else {
            ///save that there is a process which runing and save his pid
            there_is_a_process_running_in_the_front=true;
            ForegroundCommand* ret= new ForegroundCommand(args, len, copy_cmd_line);
            this->front_cmd=ret;
            return ret;
        }

    }


    // For example:
/*
  string cmd_s = string(cmd_line);
  if (cmd_s.find("pwd") == 0) {
    return new GetCurrDirCommand(cmd_line);
  }
  else if ...
  .....
  else {
    return new ExternalCommand(cmd_line);
  }
  */
    return nullptr;
}

void SmallShell::executeCommand(const char *cmd_line, ChpromptCommand &call, ChangeDirCommand &cd) {
    Command *cmd = CreateCommand(cmd_line, call, cd);
    ///to do a delete to all bulit in command because we make a new.
    if (cmd != NULL) {
        cmd->execute();
    }


    // TODO: Add your implementation here
    // for example:
    // Command* cmd = CreateCommand(cmd_line);
    // cmd->execute();
    // Please note that you must fork smash process for some commands (e.g., external commands....)
}

void ShowPidCommand::execute() {
    pid_t t = getpid();
    std::cout << "smash pid is " << t << "\n";
}


Command::Command(char **args, int len) {
    ///to do:to chang args
    this->args = new char *[len];
    for (int i = 0; i < len; i++) {
        this->args[i] = args[i];
    }
    //this->args = args;
    this->len = len;
}

JobsList::JobEntry::JobEntry(unsigned int job_id, bool is_running, Command *command, pid_t pid) {
    this->is_running = is_running;
    this->job_id = job_id;
    this->command = command;
    this->pid = pid;
    this->last_start_time = time(NULL);
    this->running_time=0;
}

void JobsList::addJob(Command *cmd, pid_t pid, bool is_running) {
    if (this->command_vector.size() == 0) {
        JobEntry new_job = JobEntry(1, is_running, cmd, pid);
        this->command_vector.push_back(new_job);
    } else {
        unsigned int max_JobId = this->command_vector.back().getJob_id();
        JobEntry new_job = JobEntry(1 + max_JobId, is_running, cmd, pid);
        this->command_vector.push_back(new_job);
    }
}

void JobsList::printJobsList() {
    removeFinishedJobs();
    for (vector<JobEntry>::iterator i = command_vector.begin();
         i != command_vector.end(); ++i) {
        unsigned int job_id = i->getJob_id();
        std::string command_name = i->getCommand();
        time_t curr_time = time(NULL);
        double diff_time = difftime(curr_time, i->getLast_start_time())+i->getRunning_time();
        double diff_time_stopped = i->getRunning_time();
        pid_t pid = i->getpid();
        if (i->getIs_running()) {
            cout << "[" << job_id << "] " << command_name << "& : " << pid << " " << diff_time << "\n";
            //[1] sleep 100& : 30901 18 secs
        } else {
            cout << "[" << job_id << "] " << command_name << " : " << pid << " " << diff_time_stopped << " " << "(stopped)"
                 << "\n";
            //[2] sleep 200 : 30902 11 secs (stopped)

        }
    }
}

void JobsList::removeFinishedJobs() {
    for (vector<JobEntry>::iterator i = command_vector.begin(); i != command_vector.end(); ++i) {
        if (waitpid(i.operator*().getpid(), NULL, WNOHANG) > 0) {
            command_vector.erase(i);
        }
        ///way wh need this????
        if (i->getJob_id() == 0) {
            return;
        }
    }
}

JobsList::JobEntry *JobsList::getLastJob(pid_t *lastJobPId) {
    *lastJobPId = command_vector.back().getpid();
    return &command_vector.back();
}

JobsList::JobEntry *JobsList::getJobById(int jobId) {
    for (vector<JobEntry>::iterator i = command_vector.begin(); i != command_vector.end(); ++i) {
        unsigned int job_id_iter = i->getJob_id();
        if (job_id_iter == jobId) {
            return &(i.operator*());
        }
    }
    return nullptr;
}

void JobsList::removeJobById(int jobId) {
    for (vector<JobEntry>::iterator i = command_vector.begin(); i != command_vector.end(); ++i) {
        unsigned int job_id_iter = i->getJob_id();
        if (job_id_iter == jobId) {
            command_vector.erase(i);
            return;
        }
    }
}
void JobsList::fgCommand(int jobId) {
    //whitout jod id
    if (jobId == 0) {
        pid_t lastJobPId = -1;
        pid_t *prtLastJobPId = &lastJobPId;

        if (command_vector.empty()) {
            cout << "smash error: fg: jobs list is empty" << "\n";
            return;
        } else {
            JobEntry *take_this_job_to_foreground = getLastJob(prtLastJobPId);
            //the last job is BackgroundCommand
            if (take_this_job_to_foreground->getIs_running()) {
                cout << take_this_job_to_foreground->getCommand() << "& : " << take_this_job_to_foreground->getpid()
                     << "\n";
            } else {
                //the last job is foregroundCommand
                time_t curr_time=time(NULL);
                take_this_job_to_foreground->SetIs_running(true);
                take_this_job_to_foreground->setLast_start_time(curr_time);
                cout << take_this_job_to_foreground->getCommand() << ": " << take_this_job_to_foreground->getpid()
                     << "\n";
            }
            waitpid(*prtLastJobPId, nullptr, 0);
            removeJobById(command_vector.back().getJob_id());
            return;
        }
    }
    //find the job how have this job id
    for (vector<JobEntry>::iterator i = command_vector.begin(); i != command_vector.end(); ++i) {
        unsigned int job_id_iter = i->getJob_id();
        if (job_id_iter == jobId) {
            if (i->getIs_running()) {
                cout << i->getCommand() << "& : " << i->getpid() << "\n";
                waitpid(i->getpid(), nullptr, 0);
                command_vector.erase(i);
                return;

            }
                //the  job is foregroundCommand
            else if (i->getIs_running() == false) {
                cout << i->getCommand() << " : " << i->getpid() << "\n";
                kill(i->getpid(), SIGCONT);
                time_t curr_time=time(NULL);
                i->SetIs_running(true);
                i->setLast_start_time(curr_time);
                waitpid(i->getpid(), nullptr, 0);
                command_vector.erase(i);
                return;
            }
        }

    }
    //do'nt find this job id
    cout << "smash error: fg: job-id " << jobId << " does not exist" << "\n";
}
//void JobsList::fgCommand(int jobId) {
//    //whitout jod id
//    if (jobId == 0) {
//        pid_t lastJobPId = -1;
//        pid_t *prtLastJobPId = &lastJobPId;
//
//        if (command_vector.empty()) {
//            cout << "smash error: fg: jobs list is empty" << "\n";
//            return;
//        } else {
//            JobEntry *take_this_job_to_foreground = getLastJob(prtLastJobPId);
//            ///to do :print what we need like the example
//            if (take_this_job_to_foreground->getIs_running()) {
//                cout << take_this_job_to_foreground->getCommand() << "& : " << take_this_job_to_foreground->getpid()
//                     << "\n";
//            } else {
//                cout << take_this_job_to_foreground->getCommand() << ": " << take_this_job_to_foreground->getpid()
//                     << "\n";
//            }
//            waitpid(*prtLastJobPId, nullptr, 0);
//            removeJobById(command_vector.back().getJob_id());
//            return;
//        }
//    }
//    //find the job how have this job id
//    for (vector<JobEntry>::iterator i = command_vector.begin(); i != command_vector.end(); ++i) {
//        unsigned int job_id_iter = i->getJob_id();
//        if (job_id_iter == jobId) {
//            if (i->getIs_running()) {
//                cout << i->getCommand() << "& : " << i->getpid() << "\n";
//                waitpid(i->getpid(), nullptr, 0);
//                command_vector.erase(i);
//                return;
//
//            } else if (i->getIs_running() == false) {
//                cout << i->getCommand() << " : " << i->getpid() << "\n";
//                kill(i->getpid(), SIGCONT);
//                waitpid(i->getpid(), nullptr, 0);
//                command_vector.erase(i);
//                return;
//            }
//        }
//
//    }
//    //do'nt find this job id
//    cout << "smash error: fg: job-id " << jobId << " does not exist" << "\n";
//}

void JobsList::killCommand(int JobId, int signum) {
    JobEntry *jobEntry = this->getJobById(JobId);
    if (jobEntry == nullptr) {
        cout << "smash error: kill: job-id " << JobId << " does not exist\n";
    } else {
        int ret = kill(jobEntry->getpid(), signum);
        if (ret == -1) {
            cout << "ERROR"; ////fix!!
        } else {
            ///case is stop signal
            if (signum == 19) {
                time_t curr_time=time(NULL);
                jobEntry->SetIs_running(false);
                jobEntry->set_running_time(jobEntry->getRunning_time()+(difftime(curr_time,jobEntry->getLast_start_time())));
            }
            if (signum == 18) {
                time_t curr_time=time(NULL);
                jobEntry->SetIs_running(true);
                jobEntry->setLast_start_time(curr_time);
            }
            cout << "signal number " << signum << " was sent to pid " << jobEntry->getpid() << "\n";
        }
    }
}

JobsList::JobEntry *JobsList::getLastStoppedJob(int *jobId) {
    for (int i = command_vector.size() - 1; i >= 0; --i) {
        bool is_running_iter = command_vector[i].getIs_running();
        if (is_running_iter == false) {
            *jobId = command_vector[i].getJob_id();
            return &command_vector[i];
        }
    }

    return nullptr;
}
void JobsList::bgCommand(int jobId) {
//whitout jod id
    if (jobId == 0) {
        int jobIdOfLastJobThatStop = -1;
        int *prtJobIdOfLastJobThatStop = &jobIdOfLastJobThatStop;
        JobEntry *jobEn = getLastStoppedJob(prtJobIdOfLastJobThatStop);
        //not stop job os found
        if (jobEn == nullptr) {
            cout << "smash error: bg: there is no stopped jobs to resume\n";
            return;
        } else {
            //last stop job is found
            cout << jobEn->getCommand() << ": " << jobEn->getpid() << "\n";
            kill(jobEn->getpid(), SIGCONT);
            //the last job is foregroundCommand
            time_t curr_time=time(NULL);
            jobEn->SetIs_running(true);
            jobEn->setLast_start_time(curr_time);
            return;
        }
    } else {
        //find the job how have this job id
        for (vector<JobEntry>::iterator i = command_vector.begin(); i != command_vector.end(); ++i) {
            unsigned int job_id_iter = i->getJob_id();
            if (job_id_iter == jobId) {
                if (i->getIs_running()) {
                    cout << "smash error: bg: job-id " << jobId << " is already running in the background\n";
                    return;
                } else {
                    cout << i->getCommand() << " : " << i->getpid() << "\n";
                    kill(i->getpid(), SIGCONT);
                    time_t curr_time=time(NULL);
                    i->SetIs_running(true);
                    i->setLast_start_time(curr_time);
                    return;
                }
            }
        }
        cout << "smash error: bg: job-id " << jobId << " does not exist\n";

    }
}
//void JobsList::bgCommand(int jobId) {
////whitout jod id
//    if (jobId == 0) {
//        int jobIdOfLastJobThatStop = -1;
//        int *prtJobIdOfLastJobThatStop = &jobIdOfLastJobThatStop;
//        JobEntry *jobEn = getLastStoppedJob(prtJobIdOfLastJobThatStop);
//        //not stop job os found
//        if (jobEn == nullptr) {
//            cout << "smash error: bg: there is no stopped jobs to resume\n";
//            return;
//        } else {
//            //last stop job is found
//            cout << jobEn->getCommand() << ": " << jobEn->getpid() << "\n";
//            kill(jobEn->getpid(), SIGCONT);
//            jobEn->SetIs_running(true);
//            return;
//        }
//    } else {
//        //find the job how have this job id
//        for (vector<JobEntry>::iterator i = command_vector.begin(); i != command_vector.end(); ++i) {
//            unsigned int job_id_iter = i->getJob_id();
//            if (job_id_iter == jobId) {
//                if (i->getIs_running()) {
//                    cout << "smash error: bg: job-id " << jobId << " is already running in the background\n";
//                    return;
//                } else {
//                    cout << i->getCommand() << " : " << i->getpid() << "\n";
//                    kill(i->getpid(), SIGCONT);
//                    i->SetIs_running(true);
//                    return;
//                }
//            }
//        }
//        cout << "smash error: bg: job-id " << jobId << " does not exist\n";
//
//    }
//}


void ChangeDirCommand::execute() {
    if (len == 2) {
        char key[] = "-";
        char *temp = lastCd;
        lastCd = getcwd(NULL, 0);

        if (strcmp(args[1], key) == 0) {
            if (temp != NULL) {
                chdir(temp);
            } else {
                std::cout << "smash error: cd: OLDPWD not set";
            }
        } else {
            chdir(args[1]);

        }
    }
    if (len > 2) {
        std::cout << "smash error: cd: too many arguments" << "\n";
    }

    ///treat case when SYS CALL fail + case of no arg
}


///to do  fork and wait if foroword
////to do fork and not wait in background

ForegroundCommand::ForegroundCommand(char **arg, int
len, char *cmd_line) : ExternalCommand(arg, len, cmd_line) {
    pid_t p = fork();
    ///to do:to understand hoe to get the time in this function
    //start_time=time();
    if (p > 0) {
        son = p;
        // parent waits for child
        wait(NULL);
    } else {
        if(p==0) {
            setpgrp();
            const char path[] = "/bin/bash";
            char *const args_to_execv[] = {(char *) "bash", (char *) "-c", cmd_line, nullptr};
            int ret = execv(path, args_to_execv);
            if (ret == -1) {
                perror("smash error: execv failed");
            }
            exit(0);
        }
    }

}

///to do
void ForegroundCommand::execute() {

}


BackgroundCommand::BackgroundCommand(char **arg, int
len, char *cmd_line) : ExternalCommand(arg, len, cmd_line) {
    pid_t p = fork();
    ///to do:to understand hoe to get the time in this function
    //start_time=time();
    if (p > 0) {
        son = p;

    }
        // parent don't waits for child
    else {
        if(p==0) {
            setpgrp();
            char path[] = "/bin/bash";
            char *args_to_execv[] = {(char *) "bash", (char *) "-c", cmd_line, nullptr};
            int ret = execv(path, args_to_execv);
            if (ret == -1) {
                perror("smash error: execv failed");
            }
            exit(0);
        }
    }
}

///to do
void BackgroundCommand::execute() {

}
