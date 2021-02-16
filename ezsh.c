/*
    Easy Shell
    CPTR 365 - Operating Systems
    Miro Manestar | February 16, 2021
*/

#include "types.h"
#include "user.h"
#include "fcntl.h"

int get_cmd(char *buf, int nbuf)
{
  printf(2, "mirom$ "); //Modifed to have my name
  memset(buf, 0, nbuf);
  gets(buf, nbuf);
  if(buf[0] == 0) // EOF
    return -1;
  return 0;
}

void ezsh_loop(void) {
    static char buf[100];

    while (1) {
        printf(2, "EZ$ ");
        memset(buf, 0, sizeof(buf));
        gets(buf, sizeof(buf));
        printf(2, "%c\n", buf[0]);
        wait();
    }
}

int main(int argc, char **argv) {
    
    ezsh_loop();
}