#include <string.h>  /* strcpy */
#include <stdlib.h>  /* malloc */
#include <stdio.h>   /* printf */
#include <stdbool.h>
#include <stdarg.h>

#include <dlfcn.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <dirent.h>
#include <sys/stat.h>
#include <sys/types.h>


#include "utility/log.h"
#include "profile/appstatus.h"

// 利用函数stat就可以获取执行文件/system/bin/su程序的属性（包括特殊权限位）
//我们来看一下su程序的属性：
//
// shell@android:/data # ls /system/bin/su -l
// -rwsr-sr-x root     shell       97385 2014-03-04 11:33 su
// 其中的8进制数值st_mode=106755对应的就是-rwsr-sr-x，
// 只看最后的755对应的rwxr-xr-x，755前面的6等于4+2，
// 所以在755中的owner和group的执行位会变成s，也就是变成rwsr-sr-x。


// 判断手机是否ROOT: 判断手机是否root只需判断su文件是否在特定的目录下（适用于大部分情况）
// check if specific directory and files exist, can only be regarded as warning
int is_sufile_existed()
{
	struct stat st;

	// Su Binaries. The following list of Su binaries are often looked for on rooted devices.
	// check su binary file
	if (   stat("/system/bin/su", &st) == 0               || stat("/system/xbin/su", &st) == 0
	        || stat("/sbin/su", &st) == 0                 || stat("/system/su", &st) == 0
	        || stat("/system/bin/.ext/.su", &st) == 0     || stat("/system/usr/we-need-root/su-backup", &st) == 0
	        || stat("/system/xbin/mu", &st) == 0          || stat("/data/local/xbin/su", &st) == 0
	        || stat("/data/local/bin/su", &st) == 0       || stat("/system/sd/xbin/su", &st) == 0
	        || stat("/system/bin/failsafe/su", &st) == 0  || stat("/data/local/su", &st) == 0
	   )
	{
	    // 部分国产手机的定制系统，本身就留有/system/bin/su，针对以上的判定需要加上下面这一条：
        // 若su文件存在，可以查看文件的可执行权限，已root的文件权限为: -rwxrwxrwx.
        if ( st.st_mode & S_IFREG )   // regular file, not folder
		if ( (st.st_mode & S_IXUSR)  &&   (st.st_mode & S_IXGRP)   && (st.st_mode & S_IXOTH) ) // executable
		{
	    	LOGE("found su binary file");
		    return 1;
	    }
	}



	// check if SuperUser.apk is installed
	// Superuser.apk. This package is most often looked for on rooted devices.
	// Superuser allows the user to authorize applications to run as root on the device.
	// package name for SuperUser.apk may be like com.noshufou.android.su
	// Other packages. The following list of packages are often looked for as well.
	// The last two facilitate in temporarily hiding the su binary and disabling installed applications.
	// 查看/system/app/ 下是否存如下root后常用软件，
	if (stat("/system/app/Superuser.apk", &st) == 0
		|| stat("/system/app/supersu.apk", &st) == 0
		|| stat("/system/app/superuser.apk", &st) == 0
		|| stat("/system/app/Busybox.apk", &st) == 0
		|| stat("/system/app/busybox.apk", &st) == 0
		|| stat("/system/app/.tmpsu.apk", &st) == 0
		|| stat("/system/app/Titanium.apk", &st) == 0
		|| stat("/system/app/titanium.apk", &st) == 0
		|| stat("/system/app/noshufou.apk", &st) == 0
		|| stat("/system/app/chainfire.apk", &st) == 0
		|| stat("/system/app/daemonsu.apk", &st) == 0
		|| stat("/system/app/greenify.apk", &st) == 0
		|| stat("/system/app/rootcloak.apk", &st) == 0
		|| stat("/system/app/substrate.apk", &st) == 0
		|| stat("/system/app/xposed.apk", &st) == 0
		)
    {
		LOGE("found Superuser.apk installed");
    	return 2;
    }

    if ( stat("/data/data/com.noshufou.android.su", &st) == 0
	        || stat("/data/data/com.thirdparty.superuser", &st) == 0
	        || stat("/data/data/eu.chainfire.supersu", &st) == 0
	        || stat("/data/data/com.koushikdutta.superuser", &st) == 0
	        || stat("/data/data/com.zachspong.temprootremovejb", &st) == 0
	        || stat("/data/data/com.ramdroid.appquarantine", &st) == 0)
	{
		LOGE("found Superuser.apk package");
		return 3;
	}

	return 0;
}


// check directory permissions
// Sometimes when a device has root, the permissions are changed on common directories.
int check_dir_permission()
{
	struct stat st;
	// Are the following directories writable.
	if ( (stat("/", &st) == 0 &&  st.st_mode & S_IWOTH)
	        || (stat("/data", &st) == 0 &&  st.st_mode & S_IWOTH)
	        || (stat("/system", &st) == 0 &&  st.st_mode & S_IWOTH)
	        || (stat("/system/bin", &st) == 0 &&  st.st_mode & S_IWOTH)
	        || (stat("/system/sbin", &st) == 0 &&  st.st_mode & S_IWOTH)
	        || (stat("/system/xbin", &st) == 0 &&  st.st_mode & S_IWOTH)
	        || (stat("/vendor/bin", &st) == 0 &&  st.st_mode & S_IWOTH)
	        || (stat("/sys", &st) == 0 &&  st.st_mode & S_IWOTH)
	        || (stat("/sbin", &st) == 0 &&  st.st_mode & S_IWOTH)
	        || (stat("/etc", &st) == 0 &&  st.st_mode & S_IWOTH)
	        || (stat("/proc", &st) == 0 &&  st.st_mode & S_IWOTH)
	        || (stat("/dev", &st) == 0 &&  st.st_mode & S_IWOTH)
	   )
	{
		LOGE("found system folder writable");
		return 1;

	}


    // Can we read files in /data.
    // The /data directory contains all the installed application files. By default, /data is not readable.
	// a normal case: drwxrwx--x system   system            2016-03-17 14:11 data
	if (stat("/data", &st) == 0 &&  st.st_mode & S_IROTH)
	{
		LOGE("found directory /data readable");
		return 2;
	}

	return 0;
}


//  not yet implemented:
// 尝试执行su、id命令，看能否成功执行，并且查看UID是否为root；


// API
int is_device_rooted()
{
	int status = 0;

	status = is_sufile_existed();
	status = status || check_dir_permission();

  if (status)
	{
		set_root_jailbreak_status();
	}

  return status;
}
