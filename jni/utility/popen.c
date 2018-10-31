#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <errno.h>
#include <unistd.h>
#include <paths.h>
#include <sys/param.h>
#include <sys/wait.h>


// 执行linux命令ps获取进程列表，并过滤可以进程.

// Android 不支持popen/pclose函数，因此自己参照linux实现.
// Google decided to write a "lite version" of libc called bionic.
// This library does not include popen(), therefore you can't use it.

// 完全参考Android/Linux实现: https://android.googlesource.com/platform/bionic.git/+/android-4.0.1_r1/libc/unistd/popen.c
// 主要通过pipe管道实现，具体思路如下：
//      1、使用pipe()建立管道
//      2、使用fork()创建子进程
//      3、在子进程中调用exec族函数执行命令，通过管道将结果传送至父进程
//      4、在主进程中等待子进程执行，子进程执行完成后将接收其结果，返回结果的文件指针


// 其他:
// 苹果popen.c的实现: http://opensource.apple.com//source/Libc/Libc-262/gen/popen.c

// 讨论popen、system的实现、问题及解决思路
// http://blog.chinaunix.net/uid-28856509-id-4690055.html


/*
简介
    因为linux命令的强大，因此编程时经常会需要直接引用命令的结果，或者是通过执行一个命令，通过命令返回值来做判断。
干这些事的就是system和popen接口函数，他们属于libc库，不属于系统调用
    system：执行一个shell命令，获取命令返回值，不关心命令输出
    popen：执行一个shell命令，获取命令的输出，也关心命令返回值

原理
    system：fork一个子进程，在子进程调用execl("/bin/sh","-c",cmd,(char*)0)执行命令，
		然后在父进程中waitpid，等待子进程结束，并获取命令的返回值

		popen：比system多了一个匿名管道，分为读模式和写模式，区别就在与管道的一端是stdin还是stdout。
		然后在父进程中，管道的另一端文件描述符生成FILE*文件指针。然后对这个指针操作。
*/


static struct pid {
	struct pid *next;
	FILE *fp;
	pid_t pid;
}*pidlist;

extern char **environ;


// input:   命令
// return:  file handler
FILE * _popen(const char *program, const char *type)
{
	struct pid * volatile cur;
	FILE *iop;
	int pdes[2];
	pid_t pid;
	char *argp[] = { "sh", "-c", NULL, NULL };

	if ((*type != 'r' && *type != 'w') || type[1] != '\0')
	{
		errno = EINVAL;
		return (NULL);
	}

	if ((cur = malloc(sizeof(struct pid))) == NULL)
		return (NULL);

	if (pipe(pdes) < 0)
	{
		free(cur);
		return (NULL);
	}

	switch (pid = fork())
	{
	case -1: /* Error. */
		(void) close(pdes[0]);
		(void) close(pdes[1]);
		free(cur);
		return (NULL);
	/* NOTREACHED */
	case 0: /* Child. */
	{
		struct pid *pcur;
		/*
		 * We fork()'d, we got our own copy of the list, no
		 * contention.
		 */
		for (pcur = pidlist; pcur; pcur = pcur->next)
			close(fileno(pcur->fp));

		if (*type == 'r')
		{
			(void) close(pdes[0]);
			if (pdes[1] != STDOUT_FILENO)
			{
				(void) dup2(pdes[1], STDOUT_FILENO);
				(void) close(pdes[1]);
			}
		}
		else
		{
			(void) close(pdes[1]);
			if (pdes[0] != STDIN_FILENO)
			{
				(void) dup2(pdes[0], STDIN_FILENO);
				(void) close(pdes[0]);
			}
		}
		argp[2] = (char *) program;
		execve(_PATH_BSHELL, argp, environ);
		_exit(127);
		/* NOTREACHED */
	}
	}

	/* Parent; assume fdopen can't fail. */
	if (*type == 'r')
	{
		iop = fdopen(pdes[0], type);
		(void) close(pdes[1]);
	}
	else
	{
		iop = fdopen(pdes[1], type);
		(void) close(pdes[0]);
	}

	/* Link into list of file descriptors. */
	cur->fp = iop;
	cur->pid = pid;
	cur->next = pidlist;
	pidlist = cur;

	return (iop);
}

/*
 * pclose --
 *  Pclose returns -1 if stream is not associated with a `popened' command,
 *  if already `pclosed', or waitpid returns an error.
 */
int _pclose(FILE *iop)
{
	struct pid *cur, *last;
	int pstat;
	pid_t pid;

	/* Find the appropriate file pointer. */
	for (last = NULL, cur = pidlist; cur; last = cur, cur = cur->next)
		if (cur->fp == iop)
			break;

	if (cur == NULL)
		return (-1);

	(void) fclose(iop);

	do {
		pid = waitpid(cur->pid, &pstat, 0);
	} while (pid == -1 && errno == EINTR);

	/* Remove the entry from the linked list. */
	if (last == NULL)
		pidlist = cur->next;
	else
		last->next = cur->next;
	free(cur);

	return (pid == -1 ? -1 : pstat);
}
