#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>

#include <jni.h>

#include "utility/log.h"
#include "profile/appstatus.h"

#define THREAT_BACKDOOR_PORT_ROOT_OWNED_LISTEN               1
#define THREAT_BACKDOOR_PORT_ADB_CONNECTED                   9
#define THREAT_BACKDOOR_PORT_SCREENCAST_AIRDROID             2
#define THREAT_BACKDOOR_PORT_SCREENCAST_REMOTE_WEB_DESKTOP   3
#define THREAT_BACKDOOR_PORT_SCREENCAST_ANMONITOR            4
#define THREAT_BACKDOOR_PORT_VNC_SERVER                      5
#define THREAT_BACKDOOR_PORT_SSH                             6
#define THREAT_BACKDOOR_PORT_TELNET                          7
#define THREAT_BACKDOOR_PORT_MERCURY_APP                     8


// 关于 ADB in emulator:
//   ADB command will restart the ADB daemon on the device and set it
//   listen to TCP/IP requests on port number 5555, which is the default port for
//   the ADB TCP/IP daemon.
//   And adb seems always bounce to 0.0.0.0
// 关于 determine if adb is used in emulator by checking tcp ports
//   检查是否有其他TCP连接到该端口.

int get_default_adbd_port()
{
	// TODO: 读取系统属性文件获取缺省端口for adb daemon
	return 5555;
}



// 扫描文件诸如 "/proc/net/tcp"中的端口判断是否有remote control漏洞
// 仅判断root owned listening port 在 Android 6.0 上产生误报.
// 可能还需要判断process name or package name.
int scan_ports(const char* netfile)
{
	int status = 0;

	int is_adbd_listening = 0;

	FILE *fp = fopen(netfile, "r");
	if (!fp)
	{
		LOGE("error open file %s: %s\n", netfile, strerror(errno));
		return 0;
	}
	// 读第一行，顺便省掉第一行标注行
	char buf[256];
	if ( fgets(buf, sizeof(buf), fp) == NULL )
	{
		LOGE("error read file %s: first line.", netfile);
		fclose(fp);
		return 0;
	}
	while (fgets(buf, sizeof(buf), fp))
	{
		int index;
		char local_addr[64], rem_addr[64];
		int local_port, rem_port, state, timer_run, uid, timeout;
		unsigned long rxq, txq, time_len, retr, inode;
		char more[512];

        //scan local port and uid
		int num = sscanf(buf, "%d: %64[0-9A-Fa-f]:%X %64[0-9A-Fa-f]:%X %X %lX:%lX %X:%lX %lX %d %d %lu %512s\n",
		       &index, local_addr, &local_port, rem_addr, &rem_port, &state,
		       &txq, &rxq, &timer_run, &time_len, &retr, &uid, &timeout, &inode, more);
		if (num < 14)
		{
			LOGE("scanf file %s error: read %d items.", netfile, num);
			continue;
		}
		LOGI("%d, %s : %d, state %d, uid %d.", index, local_addr, local_port, state, uid);


		int localIPv4 = -1; // 适用于IPv4
		if ( strlen(local_addr) > 8)
		{
			// IPv6
		}
		else
		{
			// IPv4
			sscanf(buf, "%d: %x:%x %*x:%*x %*s %*s %*s %*s %d", &index, &localIPv4, &local_port, &uid);
		}

    int found = 0;

		// 检查-1: check adb daemon listening
		int adbd_port = get_default_adbd_port();
		if ( localIPv4 == 0 && state == 0x0A && local_port == adbd_port )
		{
			// 首先判断是否有打开 listening port. // 0x0A = TCP_LISTEN
			is_adbd_listening = 1;
		}
		if ( localIPv4 != -1 && localIPv4 != 0 && is_adbd_listening && local_port == adbd_port )
		{
			// 然后判断是否有其他TCP连接到该端口。这里假定 listening TCP总是排在列表前面.
			// found tcp connection established for ADB
			found = THREAT_BACKDOOR_PORT_ADB_CONNECTED;
		}

		// 检查-2: check for root-owned listening TCP ports
		if (  // (localIP == 0 || localIP == 0100007F) &&
			 state == 0x0A && uid == 0)
		{
			// 0x0A = TCP_LISTEN
			// found = THREAT_BACKDOOR_PORT_ROOT_OWNED_LISTEN;
			// 这个可能会产生误报，需要更多判断.
			LOGE("warning: root-owned listening port %s : %d.", local_addr, local_port);
		}

		// 检查-3: check for black-listed ports
		if ( local_port == 8888 || local_port == 8889 || local_port == 8890)
		{
			// ScreenCast: Airdroid
			found = THREAT_BACKDOOR_PORT_SCREENCAST_AIRDROID;
		}
		else if ( local_port == 8999 )
		{
			// ScreenCast: Remote Web Desktop
			found = THREAT_BACKDOOR_PORT_SCREENCAST_REMOTE_WEB_DESKTOP;
		}
		else if ( local_port == 9008 )
		{
			// ScreenCast: AnMonitor
			found = THREAT_BACKDOOR_PORT_SCREENCAST_ANMONITOR;
		}
		else if ( local_port == 5900 || local_port == 5901 || local_port == 5800 || local_port == 5801)
		{
			/* VNC */
		//droid VNC server, VNC Server
			found = THREAT_BACKDOOR_PORT_VNC_SERVER;
		}
		else if ( local_port == 22)
		{
			// SSH
			found = THREAT_BACKDOOR_PORT_SSH;
		}
		else if ( local_port == 23 )
		{
			// Telnet
			found = THREAT_BACKDOOR_PORT_TELNET;
		}
		else if ( local_port == 31415)
		{
			//Mercury App
			found = THREAT_BACKDOOR_PORT_MERCURY_APP;
		}


		if (found)
		{
			LOGE("found suspicious port %s : %d, reason =%d.", local_addr, local_port, found);
			set_backdoor_port_status(found);
			status = found;

			//get package name from uid
		}

	}
	fclose(fp);
	return status;
}


int scan_port_files()
{
	int status1 = scan_ports("/proc/net/tcp");
	int status2 = scan_ports("/proc/net/udp");
	int status3 = scan_ports("/proc/net/tcp6");
	int status4 = scan_ports("/proc/net/udp6");

    return (status1 + status2 + status3 + status4);
}

// Java 代码示范: get package name from UID
// 另参考 https://android.googlesource.com/platform/cts/+/android-4.4.4_r1/tests/tests/security/src/android/security/cts/ListeningPortsTest.java

/*
// Java code
private String uidToPackage(int uid) {
        PackageManager pm = this.getContext().getPackageManager();
        String[] packages = pm.getPackagesForUid(uid);
        if (packages == null) {
            return "[unknown]";
        }
        return Arrays.asList(packages).toString();
    }
    */



/*
// JNI code, 接受一个参数Context
jstring Java_com_nlog2n_mukey_RootChecker_listPortUsing(JNIEnv *env, jobject thiz, jobject activityObject)
{
   int uid = 0; // input

   char result[100] ={0};

				//get package name from uid

				jclass activityClass = (*env)->GetObjectClass(env, activityObject);
				jmethodID getPackageManagerMethodID = (*env)->GetMethodID(env, activityClass, "getPackageManager", "()Landroid/content/pm/PackageManager;");
				//	if (getPackageManagerMethodID == 0)
				//	   LOGE("getPackageManagerMethodID error");
				jobject packageManagerObject = (*env)->CallObjectMethod(env, activityObject, getPackageManagerMethodID);

				jclass packageManagerClass = (*env)->GetObjectClass(env, packageManagerObject);
				jmethodID getNameForUidMethodID = (*env)->GetMethodID(env, packageManagerClass, "getNameForUid", "(I)Ljava/lang/String;");
				//	if (getNameForUidMethodID == 0)
				//	   LOGE("getNameForUidMethodID error");

				jobject nameObject = (*env)->CallObjectMethod(env, packageManagerObject, getNameForUidMethodID, uid);
				const char *name;
				if (nameObject != NULL)
				{
					name = (*env)->GetStringUTFChars(env, nameObject, 0);
					if (name == NULL || strlen(name) == 0)
					{
						name = "unknown";
					}
				}
				else
				{
					name = "unknown";
				}
				//append package name, using port, and protocol in to the end of result string
				sprintf(eos(result), "process:%s", name);
	return (*env)->NewStringUTF(env, result);
}
*/
