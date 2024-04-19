#include "csapp.h"

int mysystem(char *command)
{
    pid_t pid;
    int status;

    if ((pid = Fork()) == 0) {
        /* child process */
        char *argv[] = {"/bin/sh", "-c", command, NULL};
        if (execve("/bin/sh", argv, environ) == -1) {
            printf("%s: Command not found.\n", command);
            exit(0);
        }
    }
    
    /* parent process */

    /* print child pid so we can kill it */
    printf("child pid: %d\n", pid);

    if (waitpid(pid, &status, 0) > 0) {
        /* exit normally */
        if (WIFEXITED(status))
            return WEXITSTATUS(status);
        /* exit caused by signal */
        if (WIFSIGNALED(status))
            return WTERMSIG(status);
    }
}

int main(int argc, char *argv[], char *envp[])
{
    printf("normally exit with %d\n", mysystem("./exit_1"));
    printf("exit caused by signal %d\n", mysystem("./exit_2"));
    printf("exit caused by signal %d\n", mysystem("./exit_3"));
    return 0; 
}