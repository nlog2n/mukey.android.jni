#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <dlfcn.h>
#include <dirent.h>


#include "utility/log.h"

// 1. 当用Android Studio run app时会检测到 jdwp thread;

// 2. 当用Android Studio debug app时会检测到_Z25dvmDbgIsDebuggerConnectedv
//    函数返回成功.
// Note: 在Andorid 6上找不到该函数.


#define proc_pid_task_cstr                         "/proc/%d/task"
#define proc_pid_task_tid_stat_cstr                "/proc/%d/task/%s/stat"


#define JDWP_cstr                            "(JDWP)"


// 检查本进程下是否有jdwp thread.
// JWDP thread is essential to allow JWDP-type debugger
// return:  1 - found, 0 - OK
int check_jdwp_proc()
{
    //looking for subtask with (JDWP) tag
    char task[30];
    sprintf(task, proc_pid_task_cstr, getpid());
    DIR *dir = opendir(task);
    if (dir == NULL)
    {
    	LOGE("error opendir: %s\n", task);
        return 0;  // however there is error
    }

    struct dirent *de;
    while (((de = readdir(dir)) != NULL))
    {
        if (strcmp(de->d_name, ".") == 0 || strcmp(de->d_name, "..") == 0)
            continue;

        char p_subtask[30];
        sprintf(p_subtask, proc_pid_task_tid_stat_cstr, getpid(), de->d_name);
        FILE *fp = fopen(p_subtask, "r");
        if (fp == NULL)
            continue;

        char line[300];
        char p_status[30];
        char p_name[100];
        while (fgets(line, sizeof line, fp) != NULL)
        {
            sscanf(line, "%*s %99s %29s", p_name, p_status);
            if (strstr(p_status, ")") != NULL)
                sscanf(line, "%*s %99s %*s %*s", p_name);
            if (!strcmp(p_name, JDWP_cstr))
            {
                fclose(fp);
                closedir(dir);

                LOGE("jdwp thread found: %s, %s", p_subtask, line);
                return 1;
            }
        }

        fclose(fp);
    }

    closedir(dir);
    return 0;
}


// for Android 5 and below
#define dvmDbgIsDebuggerConnected_cstr          "dvmDbgIsDebuggerConnected"
#define _Z25dvmDbgIsDebuggerConnectedv_cstr     "_Z25dvmDbgIsDebuggerConnectedv"
//
#define _ZN3art3Dbg16IsDebuggerActiveEv_cstr    "_ZN3art3Dbg16IsDebuggerActiveEv"


// for android 6
// 以下C函数均未找到 in libart.so
#define _ZN3artL27VMDebug_isDebuggerConnectedEP7_cstr  "_ZN3artL27VMDebug_isDebuggerConnectedEP7"
#define _ZN3artL26VMRuntime_isDebuggerActiveEP7_cstr   "_ZN3artL26VMRuntime_isDebuggerActiveEP7"
#define _ZN3art3Dbg16IsJdwpConfiguredEv_cstr   "_ZN3art3Dbg16IsJdwpConfiguredEv"

// 检查是否有jdb attach.
int is_java_isDebuggerConnected()
{
    int result = 0;

    // 获取函数原型, 以上两个函数原型一样
    bool (*func_ptr)();

    func_ptr = dlsym(RTLD_DEFAULT, dvmDbgIsDebuggerConnected_cstr);
    if ( func_ptr != NULL && func_ptr() )
    {
    	// 找到该函数, 调用该函数结果
    	LOGE("found jdb attach: %s\n", dvmDbgIsDebuggerConnected_cstr);
    	result += 2;
    }

    // 其他函数类似
    func_ptr = dlsym(RTLD_DEFAULT, _Z25dvmDbgIsDebuggerConnectedv_cstr);
    if ( func_ptr != NULL && func_ptr() )
    {
    	// 找到该函数, 调用该函数结果. 在Android 4.2.2上只有该函数找到了.
    	LOGE("found jdb attach: %s\n", _Z25dvmDbgIsDebuggerConnectedv_cstr);
    	result += 4;
    }

    func_ptr = dlsym(RTLD_DEFAULT, _ZN3art3Dbg16IsDebuggerActiveEv_cstr);
    if ( func_ptr != NULL && func_ptr() )
    {
    	// 找到该函数, 调用该函数结果
    	LOGE("found jdb attach: %s\n", _ZN3art3Dbg16IsDebuggerActiveEv_cstr);
    	result += 8;
    }

    ///////// for android 6
    func_ptr = dlsym(RTLD_DEFAULT, _ZN3art3Dbg16IsJdwpConfiguredEv_cstr);
    if (func_ptr != NULL)
    {
        LOGE("found func: %s\n",_ZN3art3Dbg16IsJdwpConfiguredEv_cstr);
    }
    if ( func_ptr != NULL && func_ptr() )
    {
    	// 找到该函数, 调用该函数结果
    	LOGE("found jdb attach: %s\n", _ZN3art3Dbg16IsJdwpConfiguredEv_cstr);
    	result += 16;
    }


    func_ptr = dlsym(RTLD_DEFAULT, _ZN3artL27VMDebug_isDebuggerConnectedEP7_cstr);
    if (func_ptr != NULL)
    {
        LOGE("found func: %s\n",_ZN3artL27VMDebug_isDebuggerConnectedEP7_cstr);
    }
    if ( func_ptr != NULL && func_ptr() )
    {
    	// 找到该函数, 调用该函数结果
    	LOGE("found jdb attach: %s\n", _ZN3artL27VMDebug_isDebuggerConnectedEP7_cstr);
    	result += 32;
    }

    func_ptr = dlsym(RTLD_DEFAULT, _ZN3artL26VMRuntime_isDebuggerActiveEP7_cstr);
    if (func_ptr != NULL)
    {
        LOGE("found func: %s\n",_ZN3artL26VMRuntime_isDebuggerActiveEP7_cstr);
    }
    if ( func_ptr != NULL && func_ptr() )
    {
    	// 找到该函数, 调用该函数结果
    	LOGE("found jdb attach: %s\n", _ZN3artL26VMRuntime_isDebuggerActiveEP7_cstr);
    	result += 64;
    }


    return result;
}


// 相关的Java code:
/*
import android.os.Debug;
boolean status = Debug.isDebuggerConnected();
return status;
*/



// API
int is_jdwp_exist()
{
	int status = 0;

	status += check_jdwp_proc();
	status += is_java_isDebuggerConnected();

	return status;
}
