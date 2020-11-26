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

vector<string> toSeparateTheString(char *cmd_line, char *symbol) {
    char *args[COMMAND_MAX_ARGS];
    char *copy_cmd_line = const_cast<char *>(cmd_line);
    const char *const_copy = copy_cmd_line;
//    int len = _parseCommandLine(const_copy, args);
    vector<string> cmdParsed;
    if (symbol == NULL) {
        return cmdParsed;
    }
    string line = cmd_line;
    size_t index = line.find(symbol);
    int len1 = _parseCommandLine(line.substr(0, index).c_str(), args);
    string time_string;
    if(len1>0) {
        string cmd1 = args[0];
        for (int i = 1; i < len1; ++i) {
            cmd1 += " ";
            cmd1 += args[i];
        }
        cmdParsed.push_back(cmd1);
        cmdParsed.push_back(symbol);

    }else{
        time_string = symbol;
        time_string+= " ";

    }
    string stirng_symbol = symbol;
    for (int i = 0; i < len1; i++) {
        //we use malloc so we need to free it and not delete it
        free(args[i]);
    }
    //delete(args);
    int size_symbol = stirng_symbol.length();
    int len2 = _parseCommandLine(line.substr(size_symbol + index).c_str(), args);
    string cmd2;
    int i;
    if(symbol!="timeout") {
        string cmd2 = args[0];
        i = 1;
        while ( i < len2) {
            cmd2 += " ";
            cmd2 += args[i];
            i++;
        }
        cmdParsed.push_back(cmd2);
    }
    if(symbol=="timeout") {
        time_string += args[0];
        cmdParsed.push_back(time_string);
        cmd2 = args[1];
        i = 2;
        while ( i < len2) {
            cmd2 += " ";
            cmd2 += args[i];
            i++;
        }
        cmdParsed.push_back(cmd2);
    }


    for (int i = 0; i < len2; i++) {
        //we use malloc so we need to free it and not delete it
        free(args[i]);
    }

    return cmdParsed;
}


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
    this->my_job_list.removeFinishedJobs();
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

    //to make a vector of strings
    char *is_double_chets = (char *) ">>";
    char *is_chets = (char *) ">";
    char *is_pip1 = (char *) "|";
    char *is_pip2 = (char *) "|&";
    char *is_timeout = (char *) "timeout";

    string cmy_line_is_a_string = cmd_line;
    char *symbol = NULL;
    if (cmy_line_is_a_string.find(is_double_chets) != string::npos) {
        symbol = is_double_chets;
    } else if (cmy_line_is_a_string.find(is_chets) != string::npos) {
        symbol = is_chets;
    } else if (cmy_line_is_a_string.find(is_pip2) != string::npos) {
        symbol = is_pip2;
    }else if (cmy_line_is_a_string.find(is_pip1) != string::npos) {
        symbol = is_pip1;
    }
    if (cmy_line_is_a_string.find(is_timeout) != string::npos) {
        symbol = is_timeout;
    }


    if (len <= 1 + COMMAND_MAX_ARGS) {
        if (symbol == nullptr) {
            if (len <= 2 && len > 0) {
                char key1[] = "chprompt";
                if (strcmp(name_of_command, key1) == 0) {
                    call.changTheString(args, len);
                    return nullptr;
                }
            }
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
            if (len == 1) {
                char key4[] = "cd";
                if (strcmp(name_of_command, key4) == 0) {
                    cd.Set_Orig_Vals(args, len);
                    cd.execute();
                    return nullptr;
                }
            }
            char key5[] = "jobs";
            if (strcmp(name_of_command, key5) == 0) {
                this->my_job_list.printJobsList();
                return nullptr;
            }
            char key6[] = "fg";
            if (strcmp(name_of_command, key6) == 0) {
                if (len > 2) {
                    cout << "smash error: fg: invalid arguments\n";
                    return nullptr;
                }
                if (args[1] == NULL) {
                    there_is_a_process_running_in_the_front = true;
                    /////need to update front_pid_job

                    my_job_list.fgCommand(0, &this->front_cmd_pid);
                } else {
                    if (atoi(args[1]) > 0) {
                        there_is_a_process_running_in_the_front = true;
                        /////need to update front_pid_job
                        my_job_list.fgCommand(atoi(args[1]), &this->front_cmd_pid);
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
                    if (strcmp(arg1, key7) == 0) {
                        cout << "smash: sending SIGKILL signal to " << this->my_job_list.getVector().size()
                             << " jobs\n";
                        this->my_job_list.killAllJobs();
                        exit(0);
                        return nullptr;
                    }
                }
                if (len == 1) {
                    exit(0);
                    return nullptr;
                }
                exit(0);
                return nullptr;

            }
        }

/// >>, >   linoy start

        char *copy_cmd_line_to_func = const_cast<char *>(cmd_line);
        if (len > 5) {
            cerr << "smash error: invalid arguments" << endl;
            return nullptr;
        }
        vector<string> pars_string;
        if (symbol != NULL) {
            pars_string = toSeparateTheString(copy_cmd_line_to_func, symbol);
            const char *name_of_file = pars_string[2].c_str();
            if (pars_string[0] == "chprompt" || pars_string[0] == "showpid" || pars_string[0] == "pwd" ||
                pars_string[0] == "cd" || pars_string[0] == "jobs" || pars_string[0] == "kill" ||
                pars_string[0] == "fg" || pars_string[0] == "bg" || pars_string[0] == "quit") {
                if (symbol == ">" || symbol == ">>") {
                    //to all the commands
                    int stdout_copy = dup(1);
                    if (stdout_copy == -1) {
                        perror("smash error: dup failed");
                        return nullptr;
                    }
                    if (close(1) == -1) {
                        perror("smash error: close failed");
                        return nullptr;
                    }
                    int opened = 0;
                    if (symbol == ">") {
                        opened = open(name_of_file, O_WRONLY | O_CREAT | O_TRUNC, 0666);
                    } else { // ">>"
                        opened = open(name_of_file, O_WRONLY | O_CREAT | O_APPEND, 0666);
                    }
                    if (opened == -1) {
                        perror("smash error: open failed");
                        return nullptr;
                    }
                    const char *convert_to_chat = pars_string[0].c_str();
                    this->executeCommand(convert_to_chat, call, cd);
                    int check = close(1);
                    if (check == -1) {
                        perror("smash error: close failed");
                        return nullptr;
                    }
                    check = dup2(stdout_copy, 1);
                    if (check == -1) {
                        perror("smash error: dup failed");
                        return nullptr;
                    }
                    return nullptr;
                } else {
                    ///built in command but pipe
                    if (symbol == "|" || symbol == "|&") {
                        string command1 = pars_string[0];
                        string command2 = pars_string[2];
                        int fd[2];
                        int is_pipe_work = pipe(fd);
                        if (is_pipe_work == -1) {
                            perror("smash error: pipe failed");
                            return nullptr;
                        }
                        pid_t pid1 = fork();
                        if (pid1 == -1) {
                            perror("smash error: fork failed");
                            return nullptr;
                        }

                        if (pid1 == 0) {
                            int check = setpgrp();
                            if (check == -1) {
                                perror("smash error: setpgrp failed");
                                return nullptr;
                            }
                            if (command1 == "|") {
                                check = dup2(fd[1], 1);
                                if (check == -1) {
                                    perror("dup2 error: setpgrp failed");
                                    return nullptr;
                                }
                            } else {
                                check = dup2(fd[1], 2);
                                if (check == -1) {
                                    perror("dup2 error: setpgrp failed");
                                    return nullptr;
                                }
                            }
                            check = close(fd[0]);
                            if (check == -1) {
                                perror("smash error: close failed");
                                return nullptr;
                            }
                            check = close(fd[1]);
                            if (check == -1) {
                                perror("smash error: close failed");
                                return nullptr;
                            }
                            this->executeCommand(args[0], call, cd);
                            exit(0);
                            return nullptr;
                        }

                        pid_t pid2 = fork();
                        if (pid2 == -1) {
                            perror("smash error: fork failed");
                            return nullptr;
                        }
                        if (pid2 == 0) {///son2
                            int check = setpgrp();
                            if (check == -1) {
                                perror("smash error: close failed");
                                return nullptr;
                            }
                            check = dup2(fd[0], 0);
                            if (check == -1) {
                                perror("smash error: close failed");
                                return nullptr;
                            }
                            check = close(fd[0]);
                            if (check == -1) {
                                perror("smash error: close failed");
                                return nullptr;
                            }
                            check = close(fd[1]);
                            if (check == -1) {
                                perror("smash error: close failed");
                                return nullptr;
                            }
                            this->executeCommand(command2.c_str(), call, cd);
                            exit(0);
                            return nullptr;
                        }
                        int check = close(fd[0]);
                        if (check == -1) {
                            perror("smash error: close failed");
                            return nullptr;
                        }
                        check = close(fd[1]);
                        if (check == -1) {
                            perror("smash error: close failed");
                            return nullptr;
                        }
                        if (!isBackground) {
                            this->front_cmd_pid = pid1;
                            this->there_is_a_process_running_in_the_front = true;
                            this->external_front_cmd = new ExternalCommand(args, len, copy_cmd_line);
                            time_t curr_time = time(NULL);
                            int status;
                            check = waitpid(pid1, &status, WUNTRACED);
                            if (check == -1) {
                                perror("smash error: waitpid failed");
                            }
                            check = waitpid(pid2, &status, WUNTRACED);
                            if (check == -1) {
                                perror("smash error: waitpid failed");
                            }
                        }else{
                            ///pipe in the back with & at the end
                            char *copy_cmd_line_to_func2 = const_cast<char *>(command1.c_str());
                            ///edit agrs..
                            ExternalCommand *external_command_in_rid = new ExternalCommand(args, len,copy_cmd_line_to_func2);
                            my_job_list.addJob(external_command_in_rid, pid1, true);
                        }
                        return nullptr;
                    }
                }
            } else {
                if (symbol == ">" || symbol == "<<") {//not built-in, should fork
                    char *copy_cmd_line_to_func2 = const_cast<char *>(pars_string[0].c_str());

                    bool isBackground_in0 = false;
                    if (_isBackgroundComamnd(pars_string[0].c_str())) {
                        //backGround
                        _removeBackgroundSign(copy_cmd_line_to_func2);
                        isBackground_in0 = true;
                    }
                    pid_t pid = fork();
                    if (pid == -1) {
                        perror("smash error: fork failed");
                        return nullptr;
                    }
                    if (pid > 0) {//father case
                        if (isBackground_in0 == false) {
                            int status;
                            ///chack if getpid is work
                            waitpid(pid, &status, WUNTRACED);
                        }
                    }
                    if (pid == 0) { //son

                        int res = setpgrp();
                        char path[] = "/bin/bash";
                        if (res == -1) {
                            perror("smash error: setpgrp failed");
                            return nullptr;
                        }
                        //to all the commands
                        int stdout_copy = dup(1);
                        if (stdout_copy == -1) {
                            perror("smash error: dup failed");
                            return nullptr;
                        }
                        if (close(1) == -1) {
                            perror("smash error: close failed");
                            return nullptr;
                        }
                        int opened = 0;
                        if (symbol == ">") {
                            opened = open(name_of_file, O_WRONLY | O_CREAT | O_TRUNC, 0666);
                        } else { // ">>"
                            opened = open(name_of_file, O_WRONLY | O_CREAT | O_APPEND, 0666);
                        }
                        if (opened == -1) {
                            perror("smash error: open failed");
                            exit(0);
                        }

                        if (isBackground_in0) {
                            char *args_to_execv[] = {(char *) "bash", (char *) "-c", copy_cmd_line_to_func2,
                                                     nullptr};
                            int ret = execv(path, args_to_execv);
                            if (ret == -1) {
                                perror("smash error: execv failed");
                            }
                            int check = close(1);
                            if (check == -1) {
                                perror("smash error: close failed");
                                return nullptr;
                            }
                            check = dup2(stdout_copy, 1);
                            if (check == -1) {
                                perror("smash error: dup failed");
                                return nullptr;
                            }
                            exit(0);
                        } else { //frontgroundCommand
                            char *args_to_execv[] = {(char *) "bash", (char *) "-c", copy_cmd_line_to_func2,
                                                     nullptr};
                            int ret = execv(path, args_to_execv);;
                            if (ret == -1) {
                                perror("smash error: execv failed");
                            }
                            int check = close(1);
                            if (check == -1) {
                                perror("smash error: close failed");
                                return nullptr;
                            }
                            check = dup2(stdout_copy, 1);
                            if (check == -1) {
                                perror("smash error: dup failed");
                                return nullptr;
                            }
                            exit(0);
                        }

                    }
                    if (pid > 0) {//father case and isBackground_in0==true
                        //add this command into the list
                        if (isBackground_in0) {
                            ExternalCommand *external_command_in_rid = new ExternalCommand(args, len,
                                                                                           copy_cmd_line_to_func2);
                            my_job_list.addJob(external_command_in_rid, pid, true);
                        }
                    }
                    return nullptr;
                } else {


                }
            }
        }






//           // TODO: craete job and add to list.
//            return nullptr;
//        }
        if (isBackground) {
            ///check if timeout with &
            BackgroundCommand *beckCommand = new BackgroundCommand(args, len, copy_cmd_line);
            my_job_list.addJob(beckCommand, beckCommand->getpid(), true);
            return beckCommand;

        } else {



            ///save that there is a process which runing and save his pid
            pid_t p = fork();

            ///check if timeout
            if (symbol=="timeout"){
                if (p > 0) {
                    string duration_str= args[1];
                    double duration = atof(duration_str.c_str());

                    this->timeout_list.addJob_timeoutVec(p,duration,copy_cmd_line);
                    set_alarm();
                }
            }
            ///to do:to understand hoe to get the time in this function
            //start_time=time();
            if (p > 0) {
                this->front_cmd_pid = p;
                this->there_is_a_process_running_in_the_front = true;
                this->external_front_cmd = new ExternalCommand(args, len, copy_cmd_line);
                time_t curr_time = time(NULL);
                int status;
                waitpid(p, &status, WUNTRACED);
                if (WIFSTOPPED(status)) {
                    my_job_list.addJob(external_front_cmd, p, false);
                    JobEntry *jobEntry = my_job_list.getJobByPid(p);
                    jobEntry->set_running_time(jobEntry->getRunning_time() +
                                               difftime(time(NULL), curr_time));
                }
                //delte args
                this->there_is_a_process_running_in_the_front = false;
            } else {
                if (p == 0) {
                    if (symbol=="timeout") {
                        string copy="";
                        for(int i = 2; i < pars_string.size(); ++i) {
                            copy+=pars_string[i];
                        }
                    }
                    setpgrp();
                    const char path[] = "/bin/bash";
                    char *const args_to_execv[] = {(char *) "bash", (char *) "-c", copy_cmd_line, nullptr};
                    int ret = execv(path, args_to_execv);
                    if (ret == -1) {
                        perror("smash error: execv failed");
                    }
                    exit(0);
                }

            }
        }
    }

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

void SmallShell::set_alarm() {

        time_t timer;
        time(&timer);
        double min = numeric_limits<int>::max();
        bool newAlarm = false;
    for (vector<JobEntry>::iterator i = this->timeout_list.getVector().begin();
         i != this->timeout_list.getVector().end(); ++i) {
        int nextAlarm = 0;
        double duration = i->getRunning_time();
        time_t startSeconds = i->getLast_start_time();
        //time_t seconds = timer - (duration + startSeconds);
        double seconds = difftime(startSeconds, timer);
        seconds += duration;
        //cout << "the duration: " << seconds << endl;
        if (seconds < min) {
            min = seconds;
            newAlarm = true;
        }
    }
        if(newAlarm){
            // cout<< "min " << min <<endl;
            alarm(min);
        }

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

JobEntry::JobEntry(unsigned int job_id, bool is_running, Command *command, pid_t pid, double running_time = 0) {
    this->is_running = is_running;
    this->job_id = job_id;
    this->command = command;
    this->pid = pid;
    this->last_start_time = time(NULL);
    this->running_time = 0;
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
        double diff_time = difftime(curr_time, i->getLast_start_time()) + i->getRunning_time();
        double diff_time_stopped = i->getRunning_time();
        pid_t pid = i->getpid();
        if (i->getIs_running()) {
            cout << "[" << job_id << "] " << command_name << "& : " << pid << " " << diff_time << "\n";
            //[1] sleep 100& : 30901 18 secs
        } else {
            cout << "[" << job_id << "] " << command_name << " : " << pid << " " << diff_time_stopped << " "
                 << "(stopped)"
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

JobEntry *JobsList::getLastJob(pid_t *lastJobPId) {
    *lastJobPId = command_vector.back().getpid();
    return &command_vector.back();
}

JobEntry *JobsList::getJobById(int jobId) {
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

void JobsList::fgCommand(int jobId, pid_t *pid_to_update) {
    //whitout jod id
    if (jobId == 0) {
        pid_t lastJobPId = -1;
        pid_t *prtLastJobPId = &lastJobPId;
        if (command_vector.empty()) {
            cout << "smash error: fg: jobs list is empty" << "\n";
            return;
        } else {
            JobEntry *take_this_job_to_foreground = getLastJob(prtLastJobPId);
            *pid_to_update = take_this_job_to_foreground->getpid();
            //the last job is BackgroundCommand
            if (take_this_job_to_foreground->getIs_running()) {
                cout << take_this_job_to_foreground->getCommand() << "& : " << take_this_job_to_foreground->getpid()
                     << "\n";
            } else {
                //the last job is foregroundCommand
                *pid_to_update = take_this_job_to_foreground->getpid();
                kill(take_this_job_to_foreground->getpid(), SIGCONT);
                time_t curr_time = time(NULL);
                take_this_job_to_foreground->SetIs_running(true);
                take_this_job_to_foreground->setLast_start_time(curr_time);
                cout << take_this_job_to_foreground->getCommand() << ": " << take_this_job_to_foreground->getpid()
                     << "\n";
            }
            *pid_to_update = take_this_job_to_foreground->getpid();
            int status;
            waitpid(*prtLastJobPId, &status, WUNTRACED);
            if (WIFSTOPPED(status)) {
                take_this_job_to_foreground->SetIs_running(false);
                take_this_job_to_foreground->set_running_time(take_this_job_to_foreground->getRunning_time() +
                                                              difftime(time(NULL),
                                                                       take_this_job_to_foreground->getLast_start_time()));

            } else {
                removeJobById(command_vector.back().getJob_id());
            }
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
                time_t curr_time = time(NULL);
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
            cout << "ERROR kill"; ////fix!!
        } else {
            ///case is stop signal
            if (signum == SIGSTOP) {
                time_t curr_time = time(NULL);
                jobEntry->SetIs_running(false);
                jobEntry->set_running_time(
                        jobEntry->getRunning_time() + (difftime(curr_time, jobEntry->getLast_start_time())));
            }
            if (signum == 18) {
                time_t curr_time = time(NULL);
                jobEntry->SetIs_running(true);
                jobEntry->setLast_start_time(curr_time);
            }
            cout << "signal number " << signum << " was sent to pid " << jobEntry->getpid() << "\n";
        }
    }
}

JobEntry *JobsList::getLastStoppedJob(int *jobId) {
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
            time_t curr_time = time(NULL);
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
                    time_t curr_time = time(NULL);
                    i->SetIs_running(true);
                    i->setLast_start_time(curr_time);
                    return;
                }
            }
        }
        cout << "smash error: bg: job-id " << jobId << " does not exist\n";

    }
}

JobEntry *JobsList::getJobByPid(pid_t Pid) {
    for (vector<JobEntry>::iterator i = command_vector.begin(); i != command_vector.end(); ++i) {
        pid_t job_id_iter = i->getpid();
        if (job_id_iter == Pid) {
            return &(i.operator*());
        }
    }
    return nullptr;
}

void JobsList::printJobsListForKill() {
    removeFinishedJobs();
    for (vector<JobEntry>::iterator i = command_vector.begin();
         i != command_vector.end(); ++i) {
        unsigned int job_id = i->getJob_id();
        std::string command_name = i->getCommand();
        pid_t pid = i->getpid();
        if (i->getIs_running()) {
            cout << pid << ": " << command_name << "\n";
        } else {
            cout << pid << ": " << command_name << " : " << pid << "&" << "\n";

            //[2] sleep 200 : 30902 11 secs (stopped)

        }
    }
}

void JobsList::killAllJobs() {
    for (vector<JobEntry>::iterator i = command_vector.begin();
         i != command_vector.end(); ++i) {
        unsigned int job_id = i->getJob_id();
        kill(i->getpid(), SIGKILL);
        this->removeJobById(i->getJob_id());
    }

}




void JobsList::addJob_timeoutVec(pid_t pid, double duration, char *copy_cmd_line) {
    JobEntry new_job = JobEntry(1, true, nullptr , pid);
    new_job.set_running_time(duration);
    new_job.set_cmd_line(copy_cmd_line);
    this->command_vector.push_back(new_job);
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

//ForegroundCommand::ForegroundCommand(char **arg, int len, char *cmd_line, ExternalCommand** Command_to_set, pid_t* pid_to_set,JobsList& jobsList):ExternalCommand(arg,len,cmd_line) {
//    pid_t p = fork();
//    ///to do:to understand hoe to get the time in this function
//    //start_time=time();
//    if (p > 0) {
//        cout<<"line 621 pid: "<<p<<"\n";
//        son = p;
//        *pid_to_set=p;
//        *Command_to_set = new ExternalCommand(arg,len,cmd_line);
//        // parent waits for child
//        int status;
//        waitpid(p,&status,WUNTRACED);
//        if(WIFSTOPPED(status)){
//            jobsList.addJob(*Command_to_set,p,false);
//            cout<<"job was added to list: "<<p<<"\n";
//        }
//    } else {
//        if(p==0) {
//            setpgrp();
//            const char path[] = "/bin/bash";
//            char *const args_to_execv[] = {(char *) "bash", (char *) "-c", cmd_line, nullptr};
//            int ret = execv(path, args_to_execv);
//            if (ret == -1) {
//                perror("smash error: execv failed");
//            }
//            exit(0);
//        }
//    }
//
//}

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
        if (p == 0) {
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
