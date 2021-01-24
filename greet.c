//Output a message that greets the user!
//Takes either 0 or 1 arguments
//For CPTR 365 Operating Systems
//Miro Manestar | 1/22/2021

#include "types.h"
#include "stat.h"
#include "user.h"

int
main(int argc, char *argv[])
{    
    if(argc > 1){
        int i;
        
        printf(1, "Hello ");
        for (i = 1; i < argc; i++)
            printf(1, "%s%s", argv[i], i + 1 < argc ? " " : "\n");
    } else {
        printf(1, "Hello unknown user!\n");
    }
    exit();
}