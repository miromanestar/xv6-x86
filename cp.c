//Copies the contents of a file from a source 
//Takes two arguments, Src and Dst
//For CPTR 365 Operating Systems
//Miro Manestar | 1/22/2021

#include "types.h"
#include "stat.h"
#include "user.h"
#include "fcntl.h"

void cp(int fd){

}

int
main(int argc, char *argv[])
{
    //char buf[512];

    //char *src, *dest;

    if(argc < 2) {
        printf(2, "cp: Too few arguments\n");
        exit();
    }
}