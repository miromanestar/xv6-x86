/*
    Easy Shell
    CPTR 365 - Operating Systems
    Miro Manestar | February 16, 2021
*/

#include "types.h"
#include "user.h"
#include "fcntl.h"

#define MAX_BUF_SIZE 100

//A simple struct to allow me to more easily pass a command into exec & fork
struct cmd {
  char** argv;
  int argc;
};

char whitespace[] = " \t\r\n\v";
char symbols[] = "<|>&;()";

int isSpace(const char c) {
    if ( strchr(whitespace, c) == 0 )
        return 0;
    else
        return 1;
}

struct cmd* parse_cmd(const char* buf) {
    struct cmd* cmd;
    cmd = malloc(sizeof (struct cmd));
    cmd->argv = malloc(10 * sizeof (char*));
    cmd->argv[0] = malloc(30 * sizeof (char));

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

    while (1) {
        printf(2, "EZ$ ");
        memset(buf, 0, sizeof(buf));
        gets(buf, sizeof(buf));
        
        cmd = parse_cmd(buf);
        
        //Exit if user inputed "exit"
        if ( strcmp(cmd->argv[0], "exit") == 0)
            break;

        int pid = fork();
        if (pid == 0) {
            exec(cmd->argv[0], cmd->argv);
        } else {
            wait();
        }

    }

    //Free memory after exit
    for (int i = 0; i < cmd->argc; i++) //Free arg arrays
        free(cmd->argv[i]);
    free(cmd->argv); //Free pointer to args
    free(cmd); //Free cmd struct
}

int main(int argc, char **argv) {
    
    ezsh_loop();
    exit();
}