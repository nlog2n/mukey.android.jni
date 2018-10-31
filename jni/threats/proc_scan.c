#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include "threats/validate_func_ptr.h"
#include "utility/proc_pid_maps.h"
#include "utility/popen.h"
#include "utility/log.h"
#include "profile/appstatus.h"


// 执行linux命令ps获取进程列表，并过滤可以进程.


// JNIEXPORT jstring      Java_com_nlog2n_mukey_RootChecker_checkForVncSshTelnet(JNIEnv *env, jobject thiz);
// jstring Java_com_nlog2n_mukey_RootChecker_checkForVncSshTelnet(JNIEnv *env, jobject thiz)
int scan_processes()
{
	int status = 0;

	if (!is_funcaddr_valid(&_popen, lib_name_for_check))
	{

	}


	// Open the command for reading.
	FILE *fp;
	fp = _popen("ps", "r");
	if (fp == NULL)
		{
			LOGE("failed to run command <ps>.");
				return 0;
		}

	char u_name[11];  //u_name is user name (like app54)
	char p_name[200]; //p_name is path name (like /data/data/{app}/files/dropbear)
	char line[200];

	// Read the output a line at a time - output it.
	while (fgets(line, sizeof line, fp) != NULL)
	{
		int pid, ppid, found_daemon = 0;
		sscanf(line, "%s %d %d %*s %*s %*s %*s %*s %*s %s", u_name, &pid, &ppid, p_name);

#define THREAT_BACKDOOR_PROCESS_SSHD                1
#define THREAT_BACKDOOR_PROCESS_DROPBEAR            2
#define THREAT_BACKDOOR_PROCESS_TELNETD             3
#define THREAT_BACKDOOR_PROCESS_ANDROIDVNCSERVER    4
#define THREAT_BACKDOOR_PROCESS_ANDROID_VNCS        5
#define THREAT_BACKDOOR_PROCESS_PIXEL_BETA          6
#define THREAT_BACKDOOR_PROCESS_ANDROSS             7


		//SSH
		if (strstr(line, "dropbear") != NULL || strstr(line, "sshd"))
		{
			found_daemon = 1;
			set_backdoor_process_status(THREAT_BACKDOOR_PROCESS_SSHD);
		}
		//Telnet
		else if (strstr(line, "telnetd") != NULL)
		{
			found_daemon = 1;
			set_backdoor_process_status(THREAT_BACKDOOR_PROCESS_TELNETD);
		}
		//VNCServer apps
		else if (strstr(line, "androidvncserver") != NULL || strstr(line, "android_vncs") || strstr(line, "pixel_beta"))
		{
			found_daemon = 1;
			set_backdoor_process_status(THREAT_BACKDOOR_PROCESS_ANDROIDVNCSERVER);
		}
		//androSS (screenshot apps)
		else if (strstr(line, "androSS") != NULL)
		{
			found_daemon = 1;
			set_backdoor_process_status(THREAT_BACKDOOR_PROCESS_ANDROSS);
		}

		//SuperSU
		else if ((strstr(line, ":mount:master") != NULL) && ppid == 1)
		{
// example:
// root      2980  1     4004   140   ffffffff 00000000 S daemonsu:mount:master
// root      3232  1     8100   432   ffffffff 00000000 S daemonsu:master
			found_daemon = 1;
			set_root_jailbreak_status();
		}

    // summary
		if (found_daemon)
		{
			status = found_daemon;
			LOGE("found daemon process:%s, user:%s, pid:%d.", p_name, u_name, pid);
		}
	}

	// close
	_pclose(fp);

	return status;
}
