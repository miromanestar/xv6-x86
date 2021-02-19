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
  char* c;
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
    cmd->c = malloc(100 * sizeof (char));
    cmd->argv = malloc(10 * sizeof (char*));
    cmd->argv[0] = malloc(30 * sizeof (char));

    int arg = 0;
    for (int i = 0; buf[i]; i++) {
        if (isSpace(buf[i])) {
            arg++;
            cmd->argv[arg] = malloc(30 * sizeof (char)); 
            continue;
        }
        if (arg == 0) {
            cmd->c[i] = buf[i];
            continue;
        }

        cmd->argv[arg][i] = buf[i];
    }

    cmd->argc = arg;
    return cmd;
}

void ezsh_loop(void) {
    static char buf[MAX_BUF_SIZE];

    while (1) {
        printf(2, "EZ$ ");
        memset(buf, 0, sizeof(buf));
        gets(buf, sizeof(buf));
        
        struct cmd* cmd = parse_cmd(buf);
        
        printf(2, cmd->c);

        for (int i = 1; i < cmd->argc; i++)
            printf(2, "%s\n", cmd->argv[i]);

        //Free each individual item in struct
        //wait();
    }
}

int main(int argc, char **argv) {
    
    ezsh_loop();
}