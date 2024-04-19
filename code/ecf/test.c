#include "csapp.h"

int main() 
{
    int pid;
    int pgid = getpgrp();
    printf("pgid: %d\n", pgid);

    if ((pid = Fork()) == 0) {
        printf("pgid: %d\n", pgid);
        setpgid(0, 0);
        printf("pgid: %d\n", getpgrp());
    }
    else {
        waitpid(-1, NULL, 0);
        printf("pid: %d\n", pid);
    }

    return 0;
}