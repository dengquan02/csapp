# CSAPP - Chapter 8 Homework

#### 8.9

| process pair | concurrent? |
| :----------- | :---------: |
| AB           |     No      |
| AC           |     Yes     |
| AD           |     Yes     |
| BC           |     Yes     |
| BD           |     Yes     |
| CD           |     Yes     |



#### 8.10

A. call once, return twice

fork

B. call once, never return

longjmp, execve

C. call once, return 1 or more times

setjmp



#### 8.11

4 lines

```shell
                    c
                +-------+
                |    "hello"
                |    
            c   |   p
        +-------+-------+
        |     fork   "hello"
        |
        |           c
        |       +-------+
        |       |    "hello"
        |       |    
        |   p   |   p    
 +------+-------+-------+
main  fork    fork   "hello"
```



#### 8.12

8 lines

```shell
                    c
                +-------+---------+
                |    "hello"   "hello"
                |    
            c   |   p
        +-------+-------+---------+
        |     fork   "hello"   "hello"
        |
        |           c
        |       +-------+---------+
        |       |    "hello"   "hello"
        |       |    
        |   p   |   p    
 +------+-------+-------+---------+
main  fork    fork   "hello"   "hello"
```



#### 8.13

`x=4`
`x=3`
`x=2`



#### 8.14

3 lines

```shell
                    c
                +-------+
                |    "hello"
                |    
            c   |   p
        +-------+-------+
        |     fork   "hello"
        |
        |   p   
 +------+-------+-------+
main  fork   return  "hello"
```



#### 8.15

5 lines

```shell
                    c
                +-------+---------+
                |    "hello"   "hello"
                |    
            c   |   p
        +-------+-------+---------+
        |     fork   "hello"   "hello"
        |
        |
        |   p   
 +------+-------+-------+
main  fork   return  "hello"
```



#### 8.16

`counter = 2`

child process has its own counter.



#### 8.17

`hello   0  1  Bye   2   Bye`
`hello   1  0  Bye   2   Bye`
`hello   1  Bye  0   2   Bye`



#### 8.18

```shell
                    c
                +-------+---------+
                |      "0"     exit "2"
                |    
            c   |   p
        +-------+-------+---------+
        |     fork     "1"     exit "2"
        |   (atexit)
        |           c
        |       +-------+---------+
        |       |      "0"      exit
        |       |    
        |   p   |   p    
 +------+-------+-------+---------+a
main  fork    fork     "1"      exit
```

2 must be behind 0/1.

B & D is impossible.



#### 8.19

2^n



#### 8.20

我所使用的是shell程序为bash而非csh，因此

```bash
linux> setenv COLUMNS 40
linux> unsetenv COLUMNS
```

应改为

```bash
linux> export COLUMNS=40
linux> unset COLUMNS
```

```c
#include "csapp.h"

int main(int argc, char *argv[], char *envp[])
{
    if (execve("/bin/ls", argv, envp) == -1) {
        printf("%s: Command not found.\n", argv[0]);
        exit(0);
    }
}
```

实际上暂时未能实现按 COLUMNS 显示的功能。



#### 8.21

`abc`

or

`bac`



#### 8.22

```c
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
```

```c
/* $begin exit_1 */
#include "csapp.h"

int main(int argc, char* argv[]) {
    exit(10);
}
/* $end exit_1 */
```

```c
/* $begin exit_2 */
#include "csapp.h"

int main(int argc, char* argv[]) {
    while(1) ;
}
/* $end exit_2 */
```

```c
/* $begin exit_3 */
#include "csapp.h"

int main(int argc, char* argv[]) {
    Kill(getpid(), SIGKILL);
}
/* $end exit_3 */
```

When running `./mysystem`, it runs `./exit_1` firstly, then running `./wait_2` and stuck here.

<img src="https://gitee.com/dengquan02/images/raw/master/image-20231110165118760.png" alt="image-20231110165118760" style="zoom:80%;" />

Open the other terminal, type:

<img src="https://gitee.com/dengquan02/images/raw/master/image-20231110165238953.png" alt="image-20231110165238953" style="zoom:80%;" />

output:

<img src="https://gitee.com/dengquan02/images/raw/master/image-20231110165252716.png" alt="image-20231110165252716" style="zoom:80%;" />

> exit_2 and exit_3 are both killed by SIGKILL signal, but the values of  `WTERMSIG(status)` in parent process are different.
>
> The signal received by exit_2 is sent by other process, and the value of  `WTERMSIG(status)`  is 9 (as is seen from `man 7 signal`).
>
> The signal received by exit_3 is sent by itself,  and the value of  `WTERMSIG(status)`  is 137 (9 + 128). 



#### 8.23

Child process sends signal 5 times. 

The first signal is received by parent process, and the second signal becomes pending signal. 

The others are sent when the signal hander is running (it takes a long time) and are discarded, because there can be at most one pending signal of a particular type at any point in time. 

if we modify code, such as removing `sleep(1);` or sending more signals, we may get different counter value.



#### 8.24

```c
/* $begin waitpid1_8.24 */
#include "csapp.h"
#define N 2
#define LEN 1000

int main() 
{
    int status, i;
    pid_t pid;

    /* Parent creates N children */
    for (i = 0; i < N; i++)                       
	if ((pid = Fork()) == 0) {  /* Child */     
        /* access address 0, cause fault */
        char* cptr = NULL;
        *cptr = 'd';
    }

    /* Parent reaps N children in no particular order */
    while ((pid = waitpid(-1, &status, 0)) > 0) {
        if (WIFEXITED(status))
            printf("child %d terminated normally with exit status=%d\n", pid, WEXITSTATUS(status));
        else if (WIFSIGNALED(status)) {
            /* method 1 */
            // printf("child %d terminated by signal %d: ", pid, WTERMSIG(status));
            // fflush(stdout);
            // psignal(WTERMSIG(status), NULL);

            /* method 2 */
            // fprintf(stderr, "child %d terminated by signal %d: ", pid, WTERMSIG(status));
            // psignal(WTERMSIG(status), NULL);

            /* method 3 */
            char buf[LEN];
            sprintf(buf, "child %d terminated by signal %d", pid, WTERMSIG(status));
            psignal(WTERMSIG(status), buf);
        }
        else
            printf("child %d terminated abnormally\n", pid);
    }

    /* The only normal termination is if there are no more children */
    if (errno != ECHILD)                        
	    unix_error("waitpid error");

    exit(0);
}
/* $end waitpid1_8.24 */
```

> void psignal(int sig, const char *s);
>
> The psignal() function displays a message on stderr consisting of the string s, a colon, a space, a string describing the signal number sig, and a trailing newline. If the string s is NULL or empty, the colon and space are omitted. If sig is invalid, the message displayed will indicate an unknown signal.



#### 8.25

```c
/* $begin tfgets */
#include "csapp.h"
#define LEN 100

jmp_buf buf;

void sigalrm_handler()
{
    siglongjmp(buf, 1);
}

char* tfgets(char* s, int size, FILE* stream)
{
    if (!sigsetjmp(buf, 1)) {
        Signal(SIGALRM, sigalrm_handler);
        alarm(5);
        return fgets(s, size, stream);
    }
    else
        return NULL;
}

int main() 
{
    char s[LEN];
    char *input = tfgets(s, LEN, stdin);

    if (input)
        printf("%s", input);
    else
        fprintf(stderr, "Time out! Nothing input\n");

    return 0;
}
/* $end tfgets */
```

> char *fgets(char *s, int size, FILE *stream);
>
> fgets() reads in at most one less than size characters from stream and stores them into the buffer pointed to by s.  Reading stops after an EOF or a newline. If a newline is read, it is stored into the buffer. A terminating null byte ('\0') is stored after the last character in the buffer.

>unsigned int alarm(unsigned int seconds);
>
>alarm() arranges for a SIGALRM signal to be delivered to the calling process in `seconds ` seconds. 
>
>If `seconds` is zero, any pending alarm is canceled. 
>
>In any event any previously set alarm() is canceled.

[字符数组与字符指针的区别 - 知乎 (zhihu.com)](https://zhuanlan.zhihu.com/p/92480428)

[char *a 与char a[] 的区别 - jihite - 博客园 (cnblogs.com)](https://www.cnblogs.com/kaituorensheng/archive/2012/10/23/2736069.html)



#### 8.26

Complete the shell lab instead of this question.

