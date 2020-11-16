#ifndef SMASH_COMMAND_H_
#define SMASH_COMMAND_H_

#include <vector>
#include <dirent.h>
#include <unistd.h>
#include <string.h>

#define COMMAND_ARGS_MAX_LENGTH (200)
#define COMMAND_MAX_ARGS (20)

class Command {
// TODO: Add your data members
protected:
    char **args;
    int len;
public:
    //Command(const char* cmd_line);
    Command(char **args, int len);

    virtual ~Command() {
        ///to do: check if we free all the elements
        for (int i = 0; i < len; i++) {
            //we use malloc so we need to free it and not delete it
            free(args[i]);
        }
        //free(*args);
    };

    virtual void execute() = 0;
    //virtual void prepare();
    //virtual void cleanup();
    // TODO: Add your extra methods if needed
};

class BuiltInCommand : public Command {

public:
    //BuiltInCommand(const char* cmd_line);
    BuiltInCommand(char **arg, int len) : Command(arg, len) {};

    virtual ~BuiltInCommand() {};
};

class ExternalCommand : public Command {
public:
    ExternalCommand(const char *cmd_line);

    ExternalCommand(char **arg, int len) : Command(arg, len) {};

    virtual ~ExternalCommand() {}

    void execute() override;
};

class PipeCommand : public Command {
    // TODO: Add your data members
public:
    PipeCommand(const char *cmd_line);

    virtual ~PipeCommand() {}

    void execute() override;
};

class RedirectionCommand : public Command {
    // TODO: Add your data members
public:
    explicit RedirectionCommand(const char *cmd_line);

    virtual ~RedirectionCommand() {}

    void execute() override;
    //void prepare() override;
    //void cleanup() override;
};

///chprompt
class ChpromptCommand : public BuiltInCommand {
private:
    std::string str;
public:
    ChpromptCommand(char **args, int len) : BuiltInCommand(args, len), str("smash> ") {};

    //ChpromptCommand(const char* cmd_line, char** plastPwd);

    virtual ~ChpromptCommand() {}

    void changTheString(char **args, int len) {
        if (len == 2) {
            str = args[1];
            str += ">";
        } else if (len == 1) {
            str = "smash> ";
        }
    }

    void execute() override {
        std::cout << str;
    }
};

///ls
class LsDirCommand : public BuiltInCommand {
private:
    struct dirent **fileListTemp;
    int num_of_files;
public:
    // TODO: Add your data members public:
    LsDirCommand(char **args, int len) : BuiltInCommand(args, len) {

        char *buffer = getcwd(NULL, 0);
        num_of_files = scandir(buffer, &fileListTemp, NULL, alphasort);
        free(buffer);

    };

    //LsDirCommand(const char* cmd_line, char** plastPwd);

    virtual ~LsDirCommand() {}

    void execute() override {
        ///i chang it to start whit 2 because the i[0]=. and i[1]= .. and we dont want to print them.
        ///we need to check if it is allways like this
        for (int i = 2; i < num_of_files; i++) {
            std::cout << fileListTemp[i]->d_name << "\n";
        }
        //free all the things that scandir is allocat
        for (int i = 0; i < num_of_files; i++) {
            free(fileListTemp[i]);
        }
        free(fileListTemp);
    }
};

///pwd
class GetCurrDirCommand : public BuiltInCommand {
private:
    char *buffer = nullptr;
public:
    GetCurrDirCommand(const char *cmd_line);

    GetCurrDirCommand(char **args, int len) : BuiltInCommand(args, len) {

        buffer = getcwd(NULL, 0);
    };

    virtual ~GetCurrDirCommand() {}

    void execute() override {
        std::cout << buffer << "\n";
        free(buffer);
    }
};

///cd
class ChangeDirCommand : public BuiltInCommand {
private:
    char *lastCd;
public:
    ChangeDirCommand(char **args, int len) : BuiltInCommand(args, len) {};

    //ChangeDirCommand(const char* cmd_line, char** plastPwd);

    virtual ~ChangeDirCommand() {}

    void execute() override {
        if(len==2) {
            char key[] = "-";
            char *temp = lastCd;
            lastCd = getcwd(NULL, 0);

            if (strcmp(args[1], key) == 0) {
                chdir(temp);
            } else {
                chdir(args[1]);

            }
        }

    }
};


///showpid

class ShowPidCommand : public BuiltInCommand {
public:
    ShowPidCommand(const char *cmd_line);

    ShowPidCommand(char **args, int len) : BuiltInCommand(args, len) {};

    virtual ~ShowPidCommand() {};

    void execute() override;
};

class JobsList;

///quit
class QuitCommand : public BuiltInCommand {
// TODO: Add your data members public:
    QuitCommand(const char *cmd_line, JobsList *jobs);

    virtual ~QuitCommand() {}

    void execute() override;
};

class JobsList {

public:
    class JobEntry {
        // TODO: Add your data members
        unsigned int job_id;
        Command *command;
    };

private:
    std::vector<JobEntry> command_vector;
    // TODO: Add your data members
public:
    JobsList();

    ~JobsList();

    void addJob(Command *cmd, bool isStopped = false);

    void printJobsList();

    void killAllJobs();

    void removeFinishedJobs();

    JobEntry *getJobById(int jobId);

    void removeJobById(int jobId);

    JobEntry *getLastJob(int *lastJobId);

    JobEntry *getLastStoppedJob(int *jobId);
    // TODO: Add extra methods or modify exisitng ones as needed
};

class JobsCommand : public BuiltInCommand {
    // TODO: Add your data members
public:
    JobsCommand(const char *cmd_line, JobsList *jobs);

    virtual ~JobsCommand() {}

    void execute() override;
};

class KillCommand : public BuiltInCommand {
    // TODO: Add your data members
public:
    KillCommand(const char *cmd_line, JobsList *jobs);

    virtual ~KillCommand() {}

    void execute() override;
};

class ForegroundCommand : public BuiltInCommand {
    // TODO: Add your data members
public:
    ForegroundCommand(const char *cmd_line, JobsList *jobs);

    virtual ~ForegroundCommand() {}

    void execute() override;
};

class BackgroundCommand : public BuiltInCommand {
    // TODO: Add your data members
public:
    BackgroundCommand(const char *cmd_line, JobsList *jobs);

    virtual ~BackgroundCommand() {}

    void execute() override;
};

// TODO: add more classes if needed 
// maybe ls, timeout ?

class SmallShell {
private:
    // TODO: Add your data members

    SmallShell();

public:
    Command *CreateCommand(const char *cmd_line, ChpromptCommand &call,ChangeDirCommand& cd);

    SmallShell(SmallShell const &) = delete; // disable copy ctor
    void operator=(SmallShell const &) = delete; // disable = operator
    static SmallShell &getInstance() // make SmallShell singleton
    {
        static SmallShell instance; // Guaranteed to be destroyed.
        // Instantiated on first use.
        return instance;
    }

    ~SmallShell();

    void executeCommand(const char *cmd_line, ChpromptCommand &call,ChangeDirCommand& cd);
    // TODO: add extra methods as needed
};

#endif //SMASH_COMMAND_H_
