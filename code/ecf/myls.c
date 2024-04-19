#include "csapp.h"

int main(int argc, char *argv[], char *envp[])
{
    if (execve("/bin/ls", argv, envp) == -1) {
        printf("%s: Command not found.\n", argv[0]);
        exit(0);
    }
}