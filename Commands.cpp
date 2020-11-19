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

    bool isBackground = false;

    if (_isBackgroundComamnd(cmd_line)) {
        //backGround
        _removeBackgroundSign(copy_cmd_line);
        isBackground = true;

    }
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
        if (isBackground) {

            BackgroundCommand *beckCommand = new BackgroundCommand(args, len, copy_cmd_line);
            my_job_list.addJob(beckCommand, beckCommand->getpid(), true);
            return beckCommand;

        } else {
            return new ForegroundCommand(args, len, copy_cmd_line);
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
    this->args = args;
    this->len = len;
}

JobsList::JobEntry::JobEntry(unsigned int job_id, bool is_running, Command *command, pid_t pid) {
    this->is_running = is_running;
    this->job_id = job_id;
    this->command = command;
    this->pid = pid;
    this->start_time = time(NULL);
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
    for (vector<JobEntry>::iterator i = command_vector.begin();
         i != command_vector.end(); ++i) {
        unsigned int job_id = i->getJob_id();
        std::string command_name = i->getCommand();
        time_t curr_time = time(NULL);
        double diff_time = difftime(curr_time, i->getTime());
        pid_t pid = i->getpid();
        if (i->getIs_running()) {
            cout << "[" << job_id << "]" << command_name << "& : " << pid << " " << diff_time << "\n";
            //[1] sleep 100& : 30901 18 secs
        } else {
            cout << "[" << job_id << "]" << command_name << " : " << pid << " " << diff_time << " " << "(stopped)"
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
    }
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
        }
    }
}


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

ForegroundCommand::ForegroundCommand(char **arg, int len, char *cmd_line) : ExternalCommand(arg, len, cmd_line) {
    pid_t p = fork();
    ///to do:to understand hoe to get the time in this function
    //start_time=time();
    if (p > 0) {
        son = p;
        // parent waits for child
        wait(NULL);
    } else {
        const char path[] = "/bin/bash";
        char *const args_to_execv[] = {(char *) "bash", (char *) "-c", cmd_line, nullptr};
        execv(path, args_to_execv);
    }

}

///to do
void ForegroundCommand::execute() {

}


BackgroundCommand::BackgroundCommand(char **arg, int len, char *cmd_line) : ExternalCommand(arg, len, cmd_line) {
    pid_t p = fork();
    ///to do:to understand hoe to get the time in this function
    //start_time=time();
    if(p > 0) {
        son = p;

    }
    // parent don't waits for child
    else {
        char path[] = "/bin/bash";
        char *args_to_execv[] = {(char *) "bash", (char *) "-c", cmd_line, nullptr};
        execv(path, args_to_execv);
    }
}

///to do
void BackgroundCommand::execute() {

}
