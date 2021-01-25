//Copies the contents of a file from a source 
//Takes two arguments, Src and Dst
//For CPTR 365 Operating Systems
//Miro Manestar | 1/22/2021

#include "types.h"
#include "stat.h"
#include "user.h"
#include "fcntl.h"

char buf[512];

int
main(int argc, char *argv[])
{
    int fd0, fd1, n;

    if (argc <= 2) { //Check if there are too few arguments
        printf(2, "cp: Too few arguments (At least two)\n\tUsage: cp src dest dest...\n");
        exit();
    }

    //For each argument after the src, output to one or more destinations
    for (int i = 2; i < argc; i++) {

        //Check if the file cannot be read (Also assign fd0 to src file and refill buffer or something)
        if ((fd0 = open(argv[1], O_RDONLY)) < 0) {
            printf(2, "cp: cannot read %s\n", argv[1]);
            exit();
        }

        if ((fd1 = open(argv[i], O_CREATE | O_WRONLY)) < 0) { //Exit if cannot create or write to file (Also assign fd1 to dest file)
            printf(2, "cp: cannot create %s", argv[i]);
            exit();
        }

        //Output src contents to destination
        while ((n = read(fd0, buf, sizeof(buf))) > 0) {
            write(fd1, buf, n);
        }  
    }

    close(fd0); close(fd1);
    exit();
}