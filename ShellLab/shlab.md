# Shell Lab: Writing Your Own Unix Shell

[TOC]

## 说明

- 严格遵循了保守的原则：Protect accesses to shared global data structures by blocking all signals.
- 特别注意这两类情况的处理：
  - Synchronizing Flows to Avoid Nasty Concurrency Bugs
  - Explicitly Waiting for Signals
- 待改进：Call only async-signal-safe functions in handlers.



## eval

```c
/*
 * eval - Evaluate the command line that the user has just typed in
 *
 * If the user has requested a built-in command (quit, jobs, bg or fg)
 * then execute it immediately. Otherwise, fork a child process and
 * run the job in the context of the child. If the job is running in
 * the foreground, wait for it to terminate and then return.  Note:
 * each child process must have a unique process group ID so that our
 * background children don't receive SIGINT (SIGTSTP) from the kernel
 * when we type ctrl-c (ctrl-z) at the keyboard.
 */
void eval(char *cmdline)
{
    char *argv[MAXARGS]; /* Argument list execve() */
    char buf[MAXLINE];   /* Holds modified command line */
    int bg;              /* Should the job run in bg or fg? */
    pid_t pid;           /* Process id */
    sigset_t mask_all, mask_one, prev_one;

    sigfillset(&mask_all);
    sigemptyset(&mask_one);
    sigaddset(&mask_one, SIGCHLD);

    strcpy(buf, cmdline);
    bg = parseline(buf, argv);
    if (argv[0] == NULL)
        return; /* Ignore empty lines */

    if (!builtin_cmd(argv)) {
        sigprocmask(SIG_BLOCK, &mask_one, &prev_one); /* Block SIGCHLD */
        /* Child: */
        if ((pid = fork()) == 0) {
            sigprocmask(SIG_SETMASK, &prev_one, NULL); /* Unblock SIGCHLD */
            setpgid(0, 0); /* Make child independent */
            if (execve(argv[0], argv, environ) < 0) {
                printf("%s: Command not found.\n", argv[0]);
                exit(0);
            }
        }
        /* Parent: Add to job list */
        sigprocmask(SIG_BLOCK, &mask_all, NULL);
        if (bg) addjob(jobs, pid, BG, cmdline);
        else addjob(jobs, pid, FG, cmdline);
        sigprocmask(SIG_SETMASK, &prev_one, NULL); /* Unblock SIGCHLD */

        /* Block until process pid is no longer the foreground process */
        if (!bg)
            waitfg(pid);
        else {
            sigprocmask(SIG_BLOCK, &mask_all, NULL);
            printf("[%d] (%d) %s", pid2jid(pid), pid, cmdline);
            sigprocmask(SIG_SETMASK, &prev_one, NULL);
        }
    }
    return;
}
```



## builtin_cmd

```c
/*
 * builtin_cmd - If the user has typed a built-in command then execute
 *    it immediately.
 */
int builtin_cmd(char **argv)
{
    sigset_t mask_all, prev_all;
    sigfillset(&mask_all);

    /* terminates the shell */
    if (!strcmp(argv[0], "quit")) 
        exit(0);
    /* lists all background jobs */
    if (!strcmp(argv[0], "jobs")) {
        sigprocmask(SIG_BLOCK, &mask_all, &prev_all);
        listjobs(jobs); /* fg job 结束之后才能运行jobs，所以不会出现前台进程 */
        sigprocmask(SIG_SETMASK, &prev_all, NULL);
        return 1;
    }
    /* 
     * The bg(fg) <job> command restarts <job> by sending it a SIGCONT signal, and then 
     * runs it in the background(foreground). The <job> argument can be either a PID or a JID.
     */
    if (!strcmp(argv[0], "bg") || !strcmp(argv[0], "fg")) {
        do_bgfg(argv);
        return 1;
    }
    // if (!strcmp(argv[0], "&")) { /* ignore singleton & */
    //     return 1;
    // }
    return 0; /* not a builtin command */
}
```



## do_bgfg

```c
/*
 * do_bgfg - Execute the builtin bg and fg commands
 */
void do_bgfg(char **argv)
{
/*
 *     ST -> FG  : fg command
 *     ST -> BG  : bg command
 *     BG -> FG  : fg command
 */    
    struct job_t * job;
    int id;
    sigset_t mask_all, prev_all;

    sigfillset(&mask_all);

    if (argv[1] == NULL) {
        printf("%s command requires PID or %%jobid argument\n", argv[0]);
        return;
    }

    if (sscanf(argv[1], "%%%d", &id) > 0) {
        sigprocmask(SIG_BLOCK, &mask_all, &prev_all);
        job = getjobjid(jobs, id);
        sigprocmask(SIG_SETMASK, &prev_all, NULL);
        if (job == NULL) {
            printf("%s: No such job\n", argv[1]);
            return;
        }
    }
    else if (sscanf(argv[1], "%d", &id) > 0) {
        sigprocmask(SIG_BLOCK, &mask_all, &prev_all);
        job = getjobjid(jobs, id);
        sigprocmask(SIG_SETMASK, &prev_all, NULL);
        if (job == NULL) {
            printf("(%d): No such process\n", id);
            return;
        }
    }
    else {
        printf("%s: argument must be a PID or %%jobid\n", argv[0]);
        return;
    }

    // /* Race: if job terminates and so is deleted after kill and before if test, the `job` will be abnormal */
    // kill(-(job->pid), SIGCONT);
    // /* Use two pause() to simulate this case */
    // // pause(); /* Child sends SIGCHLD to its parent when it continues */
    // // pause(); /* Child sends SIGCHLD to its parent when it terminates */
    // // if (job->pid == 0) printf("job has be cleared!\n");
    // if (!strcmp(argv[0], "bg")) {
    //     job->state = BG;
    //     printf("[%d] (%d) %s", job->jid, job->pid, job->cmdline);
    // }
    // else {
    //     job->state = FG;
    //     waitfg(job->pid);
    // }


    // /* Solve race 1ed(fail): shouldn't block signal before waitfg! */
    // sigprocmask(SIG_BLOCK, &mask_all, &prev_all);
    // kill(-(job->pid), SIGCONT);
    // if (!strcmp(argv[0], "bg")) {
    //     job->state = BG;
    //     printf("[%d] (%d) %s", job->jid, job->pid, job->cmdline);
    // }
    // else {
    //     job->state = FG;
    //     waitfg(job->pid);
    // }
    // sigprocmask(SIG_SETMASK, &prev_all, NULL);


    /* Solve race 2ed */
    sigprocmask(SIG_BLOCK, &mask_all, &prev_all);
    pid_t pid = job->pid;
    kill(-pid, SIGCONT);
    if (!strcmp(argv[0], "bg")) {
        job->state = BG;
        printf("[%d] (%d) %s", job->jid, pid, job->cmdline);
        sigprocmask(SIG_SETMASK, &prev_all, NULL);
    }
    else {
        job->state = FG;
        sigprocmask(SIG_SETMASK, &prev_all, NULL);
        waitfg(pid);
    }
}
```



## waitfg

如果这里用了waitpid的话那相当于sigchld_handler的代码要写多一遍，否则会出问题，lab的官方指引建议不要这么做。

- 不要在多个地方调用 `waitpid`，很容易造成竞争条件，也会造成程序过分复杂

```c
/*
 * waitfg - Block until process pid is no longer the foreground process
 */
void waitfg(pid_t pid)
{
    // /* Wasteful */
    // while (pid == fgpid(jobs)) {
    //     sleep(0); 
    // }

    // /* Too slow */
    // while (pid == fgpid(jobs)) {
    //     sleep(1); 
    // }

    // /* Race: if the SIGCHLD is received after while test and before pause, the pause will sleep forever */
    // while (pid == fgpid(jobs)) {
    //     pause(); 
    // }

    /* When the fg job changes(terminates/stops), the kernal sends a SIGCHLD to the shell. */
    sigset_t mask, prev;
    sigemptyset(&mask);
    sigaddset(&mask, SIGCHLD);

    sigprocmask(SIG_BLOCK, &mask, &prev); /* Block SIGCHLD */
    while (pid == fgpid(jobs)) { /* Use while: might receive the SIGCHLD of bg jobs or other signals */
        sigsuspend(&prev);
    }
    /* Optionally unblock SIGCHLD */
    sigprocmask(SIG_SETMASK, &prev, NULL);
}
```



## Signal handlers

```c
/*
 * sigchld_handler - The kernel sends a SIGCHLD to the shell whenever
 *     a child job terminates (becomes a zombie), or stops because it
 *     received a SIGSTOP or SIGTSTP signal. The handler reaps all
 *     available zombie children, but doesn't wait for any other
 *     currently running children to terminate.
 */
void sigchld_handler(int sig)
{
    // printf("%d catch sigchld\n", getpid());
    int olderrno = errno;
    pid_t pid;
    int status;
    sigset_t mask_all, prev_all;
    sigfillset(&mask_all);

    while ((pid = waitpid(-1, &status, WNOHANG | WUNTRACED)) > 0) {
        /* 
         * if there exists child process running, return immediately with 0
	     * if there are child processes stopped or terminated, return the pid
         */
        sigprocmask(SIG_BLOCK, &mask_all, &prev_all);
        if (WIFEXITED(status)) {
            // Exit normally
            deletejob(jobs, pid);
            // printf("job %d deleted!\n", pid);
        }
        else if (WIFSIGNALED(status)) {
            // Exit with Ctrl+C
            int jid = pid2jid(pid);
            printf("Job [%d] (%d) terminated by signal %d\n", jid, pid, WTERMSIG(status));
            deletejob(jobs, pid);
            // printf("job %d deleted!\n", pid);
        }
        else if (WIFSTOPPED(status)) {
            // Stopped with Ctrl+Z
            struct job_t * job = getjobpid(jobs, pid);
            printf("Job [%d] (%d) Stopped by signal %d\n", job->jid, pid, WSTOPSIG(status));
            job->state = ST;
        }
        sigprocmask(SIG_SETMASK, &prev_all, NULL);
    }

    errno = olderrno;
}

/*
 * sigint_handler - The kernel sends a SIGINT to the shell whenver the
 *    user types ctrl-c at the keyboard.  Catch it and send it along
 *    to the foreground job.
 */
void sigint_handler(int sig)
{
    int olderrno = errno;
    sigset_t mask_all, prev_all;
    sigfillset(&mask_all);
    
    sigprocmask(SIG_BLOCK, &mask_all, &prev_all);
    pid_t pid = fgpid(jobs);
    sigprocmask(SIG_SETMASK, &prev_all, NULL);
    if (pid == 0)
        return;
    // printf("%d catch sigint and forward it to %d\n", getpid(), -pid);
    /* Send SIGINT to the whole foreground group */
    kill(-pid, sig);
    errno = olderrno;
}

/*
 * sigtstp_handler - The kernel sends a SIGTSTP to the shell whenever
 *     the user types ctrl-z at the keyboard. Catch it and suspend the
 *     foreground job by sending it a SIGTSTP.
 */
void sigtstp_handler(int sig)
{
    int olderrno = errno;
    sigset_t mask_all, prev_all;
    sigfillset(&mask_all);
    
    sigprocmask(SIG_BLOCK, &mask_all, &prev_all);
    pid_t pid = fgpid(jobs);
    sigprocmask(SIG_SETMASK, &prev_all, NULL);
    if (pid == 0)
        return;
    // printf("%d catch sigtstp and forward it to %d\n", getpid(), -pid);
    /* Send SIGINT to the whole foreground group */
    kill(-pid, sig);
    errno = olderrno;
}
```



## 测试结果

运行 `make test01` ~ `make test16`，与 tshref.out 进行比对。

准确无误！



## 参考链接

[CSAPP实验之shell lab - 知乎 (zhihu.com)](https://zhuanlan.zhihu.com/p/89224358)

[《深入理解计算机系统》配套实验：Shell Lab - 知乎 (zhihu.com)](https://zhuanlan.zhihu.com/p/34167162)

[CS:APP配套实验5：Shell Lab笔记 - 知乎 (zhihu.com)](https://zhuanlan.zhihu.com/p/28695244)

[Shelllab_1_介绍本次任务_哔哩哔哩_bilibili](https://www.bilibili.com/video/BV1EF411h791?p=1&vd_source=a0157918f14fa34bbbb7602f620ed576)

[Shell-lab/lab5/tsh.c at master · zhoudiqiu/Shell-lab (github.com)](https://github.com/zhoudiqiu/Shell-lab/blob/master/lab5/tsh.c)

[五 Shell Lab - 简书 (jianshu.com)](https://www.jianshu.com/p/7f5e78e83a0e)