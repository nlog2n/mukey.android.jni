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


#include "utility/property_get.h"
#include "utility/proc_pid_maps.h"
#include "utility/log.h"
#include "profile/appstatus.h"



// jint Java_com_nlog2n_mukey_RootChecker_getRootStatus(JNIEnv *env, jobject obj)
int validate_func_ptr()
{
	int isFunctionNotValid = 0;  // check the integrity of 3 JNI functions implemented in the module

	// 这里给出了例子如何对JNI函数也做函数指针完整性检测:
  /*
	if (
		!is_funcaddr_valid(&Java_com_nlog2n_mukey_RootChecker_checkForVncSshTelnet, lib_name_for_check)
	 || !is_funcaddr_valid(&Java_com_nlog2n_mukey_RootChecker_findSuidSgidFiles, lib_name_for_check)
	)
	{
		LOGE("Invalid jni function pointer found!");
		isFunctionNotValid = 1;
		set_jni_func_tampered_status(isFunctionNotValid);
	}
	*/

	return  isFunctionNotValid;
}
