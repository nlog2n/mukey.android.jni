#include <string.h>  /* strcpy */
#include <stdlib.h>  /* malloc */
#include <stdio.h>   /* printf */
#include <stdbool.h>
#include <sys/cdefs.h>
#include <sys/stat.h>
#include <sys/ptrace.h>
#include <sys/types.h>
#include <zlib.h>
#include <unistd.h>
#include <paths.h>
#include <stdarg.h>
#include <limits.h>
#include <errno.h>
#include <dirent.h>
#include <pthread.h>
#include <linux/time.h>
#include <dlfcn.h>
#include <sys/wait.h>
#include <sys/file.h>
#include <fcntl.h>
#include <sys/mman.h>

// 还未整理
//int detect_gdb(void) __attribute__((constructor));
//int detect_gdb(void) {
//	if (ptrace(PTRACE_TRACEME, 0, 0, 0) == 0) {
////		LOGE("%s\n", "No PTrace");
////		LOGE("%s\n", "We r tracer");
//		ptrace(PTRACE_DETACH, 0, 0, 0);
//		return 0;
//	} else {
////		LOGE("%s\n", "PTrace is detected!!! Bye");
//		printf("Debugger detected!");
//		return 1;
//	}
//}


// 父进程检测
// 通常在使用gdb调试时，是通过启动gdb，fork出子进程后执行目标二进制文件。因此，二进制文件的父进程即为调试器。
// 我们可通过检查父进程名称来判断是否是由调试器fork。
int is_ppid_gdb_debugger()
{
	char filename[64];
	snprintf(filename, sizeof(filename), "/proc/%d/cmdline", getppid());
	FILE* fp = fopen(filename, "r");
	if (fp == NULL)
	{
		return 0;
	}

	char line[256];
	if (fgets(line, sizeof(line), fp) == NULL)
	{
		fclose(fp);
		return 0;
	}

	fclose(fp);

	if (strcmp(line, "gdb") == 0)
	{
		// printf("gdb debugger detected");
		return 1;
	}

	return 0;
}



// 当前运行进程检测
// 通过遍历/proc/<pid>/status文件中第一行进程名字实现, 与扫描/proc/<pid>/cmdline类似.
// 例如对android_server进程检测。针对这种检测只需将android_server改名就可绕过
pid_t get_pid_by_name(const char *appname)
{
	pid_t pid = 0; // not found

	DIR* dir = opendir("/proc");
	if (!dir)
	{
		perror("open /proc fail.\n");
		return 0; // error, should never happen
	}

	struct dirent *de = NULL;
	while ((de = readdir(dir)) != NULL)
	{
		// skip those file names which are not numbers (pid)
		pid = atoi(de->d_name);
		if (!pid)
			continue;

		// open file
		char filename[128];
		sprintf(filename, "/proc/%s/status", de->d_name);
		FILE* fp = fopen(filename, "r");
		if (!fp)
		{
			continue;
		}

		// read the first line
		char line[256];
		if (fgets(line, sizeof(line), fp) == NULL)
		{
			fclose(fp);
			continue;
		}

		fclose(fp);

		printf("pid %d, name %s\n", pid, line);

		// parse the name
		char szName[128];
		sscanf(line, "%*s %s", szName);
		if (strcmp(szName, appname) == 0)
		{
			// found
			closedir(dir);
			return pid;
		}
	}

	closedir(dir);
	return 0;
}


// IDA->dbgsrv目录下的android_server 拷贝到device上运行
// 此android_server需和ida配套使用.
// 查看: ps | grep android_server
int find_android_server()
{
	if (get_pid_by_name("android_server") > 0)
	{
		// set status: being debugged by IDA pro
		return 1;
	}

	return 0;
}


// 检查/proc/pid/task/tid/下status文件，是否有线程被debugger traced
// Note: stat和status文件内容类似，前者只有一行数据值，后者更易懂些
// 参考检查是否有JDWP thread的检测方法，类似。
// 文件格式参考: http://stackoverflow.com/questions/1420426/calculating-cpu-usage-of-a-process-in-linux
int check_proc_task_status()
{
	char task[30];
	sprintf(task, "/proc/%d/task", (int) getpid());
	DIR *dir = opendir(task);
	if (dir == NULL)
	{
		// printf("error opendir: %s\n", task);
		return 0;
	}

	struct dirent *de;
	while (((de = readdir(dir)) != NULL))
	{
		if (strcmp(de->d_name, ".") == 0 || strcmp(de->d_name, "..") == 0)
			continue;

		char p_subtask[30];
		sprintf(p_subtask, "/proc/%d/task/%s/stat", (int) getpid(),	de->d_name);
		FILE* fp = fopen(p_subtask, "r");
		if (!fp)
			continue;

		char line[300];
		char p_status[30];
		char p_name[100];
		char p_temp[100];
		while (fgets(line, sizeof line, fp) != NULL) 
		{
			sscanf(line, "%*s %s %s", p_name, p_status);
			if (strstr(p_status, ")") != NULL) 
			{
				sscanf(line, "%*s %s %*s %s", p_name, p_status);
			}

			printf("scanning %s %s %s\n",p_subtask, p_name, p_status);
			if (!strcmp(p_status, "T")) 
			{
				printf("found debugger on pid %s %s\n", de->d_name, p_name);
				fclose(fp);
				closedir(dir);
				return 1;
			}
		}

		fclose(fp);
	}

	closedir(dir);
	return 0;
}


#ifdef __TEST_GDB_CHECK__

int main()
{
	is_ppid_gdb_debugger();
    find_android_server();
    check_proc_task_status();

    return 0;
}

#endif