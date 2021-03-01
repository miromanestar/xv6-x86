/*
    xv6 Editor
    CPTR 365 - Operating Systems
    Miro Manestar | March 1, 2021
*/

#include "types.h"
#include "stat.h"
#include "user.h"

int main(int, char**);
int read_file(List*, char*);

typedef struct Line {
    char *line;
    Line *next;
} Line;

typedef struct List {
    Line* head;
    int count;
} List;

int main(int argc, char **argv) {
    static char inBuf[100]; //Input buffer
    int fd; //Reference to open file
    List *file = malloc(sizeof *file); //File data

    if (argc < 2) {
        printf(1, "Error: xvEdit only accepts one argument");
        exit();
    }

    printf(2, "Welcome to the xv6 Editor!\n");
    fd = read_file(file, argv[1]);
    
    //Begin primary loop
    while (1) {
        printf(2, "xvEdit> ");
        memset(inBuf, 0, sizeof inBuf);
        gets(inBuf, sizeof inBuf);
    }

    close(fd);
}

int read_file(List *list, char *file) {
    char *buf[512];
    int fd; //File descriptor (Reference to opened file)

    //Open file and print error message if it cannot be opened
    if ( (fd = open(file, 0)) < 0 ) {
        printf(1, "xvEdit: cannot open %s\n", file);
        exit();
    }

    /*
        Perhaps try reading in line by line rather than all at once?
        A linked list struct each containing a line might be a better
        solution
    */

    int n; //Current char from file
    while ( (n = read(fd, buf, sizeof buf)) > 0 ) {
        if ( write(1, buf, n) != n ) {
            printf(1, "xvEdit: write error\n");
            exit();
        }

        if ( (char) n == '\n') {

        }
    }

    if (n < 0) {
        printf(1, "xvEdit: read error\n");
        exit();
    }

    return fd;
}