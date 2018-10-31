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


#include <jni.h>

#include "rootcheck.h"
#include "utility/property_get.h"
#include "utility/proc_pid_maps.h"
#include "utility/log.h"


/*
 * Check for the usage of Android Debugger (ADB)
 * - Identify the Process ID (PID) of 'adbd'
 * - Next, check the existance of 'logcat' or 'sh'. Retrieve its Parent Process ID (PPID)
 * - Compare if the 'logcat' or 'sh' is spawned from 'adbd'
 * RETURN: 0 - ADB is not in used
 * 		   1 - ADB is in used
 */


int adbd_check()
{
	char line[300];
	char p_stat[30];
	char p_status[30];
	char p_name[100];
	struct dirent *de;
	FILE *fp;
	int process = 0, p_pid = 0, p_ppid = 0;
	int pid_adbd = 0, found_adbd = 0;

	DIR *dir = opendir("/proc");
	if (dir == NULL)
	{
		return 0;
	}

	while (((de = readdir(dir)) != NULL))
	{
		process = atoi(de-> d_name);
		if (!process || (process < 100))
		{
			continue;
		}

		sprintf(p_stat, "/proc/%d/stat", process);
		fp = fopen(p_stat, "r");
		if (fp == NULL)
		{
			closedir(dir);
			return 0;
		}

		while (fgets(line, sizeof line, fp) != NULL)
		{
			if (strstr(line, "adbd") != NULL)
			{
				sscanf(line, "%d %s %s %d", &p_pid, p_name, p_status, &p_ppid);
				if (strstr(p_status, ")") != NULL)
				{
					sscanf(line, "%d %s %*s %s %d", &p_pid, p_name, p_status, &p_ppid);
				}

				if (p_ppid == 1)
				{
					pid_adbd = p_pid;
					found_adbd = 1;
					continue;
				}
			}

			if (found_adbd == 1)
			{
				sscanf(line, "%d %s %s %d", &p_pid, p_name, p_status, &p_ppid);
				if (strstr(p_status, ")") != NULL)
				{
					sscanf(line, "%d %s %*s %s %d", &p_pid, p_name, p_status, &p_ppid);
				}

				if (!(strcmp(p_name, "logcat")) || (strstr(p_name, "sh")))
				{
					if (p_ppid == pid_adbd)
					{
						LOGI("ADBD pid %d found %s on ppid %d ", pid_adbd, p_name, p_ppid);
						fclose(fp);
						closedir(dir);
						return 1;
					}
				}
			}
		}
		fclose(fp);
	}
	closedir(dir);
	return 0;
}
