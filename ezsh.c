/*
    Easy Shell
    CPTR 365 - Operating Systems
    Miro Manestar | February 16, 2021
*/

#include "types.h"
#include "user.h"
#include "fcntl.h"

#define MAX_BUF_SIZE 100
#define MAX_ARGS 10
#define ARG_LEN 30

void ezsh_exec(char *, char **, int);

//A simple struct to allow me to more easily pass a command into exec & fork
struct cmd {
  char** argv;
  int argc;
};

char whitespace[] = " \t\r\n\v";
char symbols[] = "<|>&;()";

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

int isSpace(const char c) {
    if ( strchr(whitespace, c) == 0 )
        return 0;
    else
        return 1;
}

struct cmd* parse_cmd(const char* buf) {
    struct cmd* cmd;
    cmd = malloc(sizeof *cmd);
    cmd->argv = malloc(MAX_ARGS * sizeof cmd->argv);
    cmd->argv[0] = malloc(ARG_LEN * sizeof *cmd->argv);

    int arg = 0;
    int offset = 0;
    for (int i = 0; buf[i]; i++) {
        if (isSpace(buf[i])) {
            arg++;
            cmd->argv[arg] = malloc(30 * sizeof (char));
            offset = i + 1;
            continue;
        }

        cmd->argv[arg][i - offset] = buf[i];
    }

    cmd->argc = arg;

    return cmd;
}

void ezsh_loop(void) {
    static char buf[MAX_BUF_SIZE];
    struct cmd* cmd;

    int histLength = 2;
    struct cmd** history = malloc(histLength * sizeof cmd);
    int cmdc = 0;

    while (1) {
        printf(2, "%d | EZ$ ", cmdc);
        memset(buf, 0, sizeof(buf));
        gets(buf, sizeof(buf));
        
        printf(2, "%s\n", buf);

        cmd = parse_cmd(buf);
        
        if (cmdc >= histLength - 1) {
            histLength *= 2;
            struct cmd** temp = history;
            history = malloc(histLength * sizeof cmd);
            
            for (int i = 0; i < histLength/2; i++)
                history[i] = temp[i];

            free(temp);
        }

        history[cmdc] = cmd;
        cmdc++;

        //Exit if user inputed "exit"
        if ( strcmp(cmd->argv[0], "exit") == 0)
            break;
        else if ( isSpace(cmd->argv[0][0]) ) {
            printf(2, "Please remove extra whitespace");
            continue;
        //CD must be built into shell because it changes the state of the shell (Working directory)
        //and child processes cannot change the state of the the shell (I.e. from exec)
        } else if ( strcmp(cmd->argv[0], "cd") == 0) {
            ezsh_cd(cmd->argv);
            continue;
        } else if ( cmd->argv[0][0] == '#' ) {
            //Will recursively search until it finds base argument that doesn't start with #
            while (cmd->argv[0][0] == '#')
                cmd = history[ atoi(cmd->argv[0] + 1) ];
        }
        if ( strlen(cmd->argv[0]) - (strchr(cmd->argv[0], '&') - cmd->argv[0]) == 1 ) {
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

void ezsh_exec(char* arg, char** argv, int type) {
    //Only run argumet if is child process (PID == 0)
    if (fork() == 0)
        exec(arg, argv);

    //If the program is supposed to run in the foreground
    else if (type == 0)
        wait();
}

int main(int argc, char **argv) {

    ezsh_loop();
    exit();
}
