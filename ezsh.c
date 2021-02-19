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

    int arg = 0;
    for (int i = 0; buf[i]; i++) {
        if (isSpace(buf[i])) {
            arg++;
            continue;
        }
        if (arg == 0) {
            cmd->c[i] = buf[i];
        }
        cmd->argv[arg][i] = buf[i];
    }

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
        printf(2, cmd->argv[0]);
        printf(2, cmd->argv[1]);

        //wait();
    }
}

int main(int argc, char **argv) {
    
    ezsh_loop();
}