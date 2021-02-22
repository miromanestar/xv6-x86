/*
    Easy Shell
    CPTR 365 - Operating Systems
    Miro Manestar | February 16, 2021
*/

#include "types.h"
#include "user.h"
#include "fcntl.h"

#define MAX_BUF_SIZE 100
#define MAX_ARGS 25
#define ARG_LEN 30

//Forward declarations
int ezsh_cd(char**);
int is_space(const char);
struct cmd* parse_cmd(const char*);
void ezsh_loop(void);
int ezsh_exec(char *, char **, int);
int main(int, char**);

//A simple struct to allow me to more easily pass a command into exec & fork
struct cmd {
  char** argv;
  int argc;
};

char whitespace[] = " \t\r\n\v";

//CD must be built into shell because it changes the state of the shell (Working directory)
//and child processes cannot change the state of the the shell (I.e. from exec)
int ezsh_cd(char **args) {
    if (args[1] == 0) {
        printf(2, "cd: Missing arguments\nUsage: cd path\n");
        return 0;
    } else if (chdir(args[1]) != 0) {
        printf(2, "cd: Unknown Error");
        return 0;
    }

    return 1;
}

//Returns 1 if arg1 is included in the whitespace string
int is_space(const char c) {
    if ( strchr(whitespace, c) == 0 )
        return 0;
    else
        return 1;
}

//Parses the command into a struct to be passed into exec
struct cmd* parse_cmd(const char* buf) {
    struct cmd* cmd;
    cmd = malloc(sizeof *cmd);
    cmd->argv = malloc(MAX_ARGS * sizeof cmd->argv);
    cmd->argv[0] = malloc(ARG_LEN * sizeof *cmd->argv);

    int arg = 0; //Keeps track of arg currently being parsed
    int offset = 0; //Track index when whitespace was last encountered in buffer
    for (int i = 0; buf[i]; i++) {
        if (is_space(buf[i])) {
            arg++;

            cmd->argv[arg] = malloc(30 * sizeof (char)); //Double allocated space if more args are needed
            offset = i + 1;
            continue;
        }

        cmd->argv[arg][i - offset] = buf[i];
    }

    cmd->argc = arg;
    
    //Get rid of extra empty args so commands aren't given empty args
    for (int i = arg; i < MAX_ARGS; i++)
        cmd->argv[i] = '\0';

    return cmd;
}

void ezsh_loop(void) {
    static char buf[MAX_BUF_SIZE];
    struct cmd* cmd;

    int histLength = 2;
    struct cmd** history = malloc(histLength * sizeof cmd);
    int cmdc = 0; //Keep count of curent number of commands

    while (1) {
        printf(2, "%d | EZ$ ", cmdc);
        memset(buf, 0, sizeof(buf)); //Clear input buffer
        gets(buf, sizeof(buf)); //Fill buffer with input for sizeof(buf) chars

        cmd = parse_cmd(buf);
        
        //Reallocate more space to history if needed
        if (cmdc >= histLength - 1) {
            histLength *= 2;
            struct cmd** temp = history; //Store history in temporary pointer
            history = malloc(histLength * sizeof cmd); //Point history to newly allocated memory
            
            //Copy from old history pointer to new history pointer
            for (int i = 0; i < histLength/2; i++)
                history[i] = temp[i];

            free(temp);
        }

        history[cmdc] = cmd;
        cmdc++;

        //Exit if user inputed "exit"
        if ( strcmp(cmd->argv[0], "exit") == 0) {
            break;
        } else if ( strcmp(cmd->argv[0], "cd") == 0) {
            ezsh_cd(cmd->argv);
            continue;
        } else if ( cmd->argv[0][0] == '#' ) {
            //Will recursively search until it finds base argument that doesn't start with #
            while (cmd->argv[0][0] == '#')
                cmd = history[ atoi(cmd->argv[0] + 1) ];
        }

        //If command ends with a &
        if ( cmd->argv[0][strlen(cmd->argv[0]) - 1] == '&' ) {
            //b_arg is argv[0] stripped of the &
            //because I don't want to modify original input that's saved in the history
            char* b_arg = malloc(ARG_LEN * sizeof *cmd->argv);
            strcpy(b_arg, cmd->argv[0]);
            memset(b_arg + (strlen(b_arg) - 1), '\0', 1);

            ezsh_exec(b_arg, cmd->argv, 1);
            continue;
        }
        
        ezsh_exec(cmd->argv[0], cmd->argv, 0);

    }

    //Free memory just before exit
    for (int i = 0; i < cmdc; i++) {
        //Free items argv
        for (int k = 0; k < history[i]->argc; k++)
            free(history[i]->argv[k]);
        free(history[i]->argv);
        free(history[i]);
    }
    free(history);
}

int ezsh_exec(char* arg, char** argv, int type) {
    //Only run argumet if is child process (PID == 0)
    if (fork() == 0) {
        exec(arg, argv);

        //If exec fails, we need to kill the child process.
        printf(2, "exec %s failed\n", argv[0]);
        exit();
    }

    //If the program is supposed to run in the foreground
    if (type == 0)
        wait();
    return 0;
}

int main(int argc, char **argv) {
    ezsh_loop();
    exit();
}
