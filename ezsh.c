/*
    Easy Shell
    CPTR 365 - Operating Systems
    Miro Manestar | February 16, 2021
*/

#include "types.h"
#include "user.h"
#include "fcntl.h"

//A simple struct to allow me to more easily pass a command into exec & fork
struct cmd {
  char* c;
  char** argv;
};

char whitespace[] = " \t\r\n\v";
char symbols[] = "<|>&;()";

struct cmd* parse_cmd(char* buf) {
    struct cmd* cmd;
    cmd = malloc(sizeof *cmd);

    return cmd;
}

void ezsh_loop(void) {
    static char buf[100];

    while (1) {
        printf(2, "EZ$ ");
        memset(buf, 0, sizeof(buf));
        gets(buf, sizeof(buf));
        printf(2, "\n%p\n", strchr(buf, ' '));
        //wait();
    }
}

int main(int argc, char **argv) {
    
    ezsh_loop();
}