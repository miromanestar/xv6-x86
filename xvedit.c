/*
    xv6 Editor
    CPTR 365 - Operating Systems
    Miro Manestar | March 1, 2021
*/

#include "types.h"
#include "stat.h"
#include "user.h"

int main(int, char**);
int read_file(char*, char*);

int main(int argc, char **argv) {
    static char inBuf[100]; //Input buffer
    static char fBuf[512]; //File buffer
    int fd; //Reference to open file

    if (argc < 2) {
        printf(1, "Error: xvEdit only accepts one argument");
        exit();
    }

    printf(2, "Welcome to the xv6 Editor!\n");
    fd = read_file(fBuf, argv[1]);
    
    //Begin primary loop
    while (1) {
        printf(2, "xvEdit> ");
        memset(inBuf, 0, sizeof inBuf);
        gets(inBuf, sizeof inBuf);
    }

    close(fd);
}

int readFile(char *buf, char *file) {
    int fd; //File descriptor (Reference to opened file)

    //Open file and print error message if it cannot be opened
    if ( (fd = open(file, 0)) < 0 ) {
        printf(1, "xvEdit: cannot open %s\n", file);
        exit();
    }

    /*
        Perhaps try reading in line by line rather than all at once?
    */

    int n; //Current char from file
    while ( (n = read(fd, buf, sizeof buf)) > 0 ) {
        if ( write(1, buf, n) != n ) {
            printf(1, "xvEdit: write error\n");
            exit();
        }
    }

    if (n < 0) {
        printf(1, "xvEdit: read error\n");
        exit();
    }

    return fd;
}