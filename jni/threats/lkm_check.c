#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <unistd.h>
#include <dlfcn.h>
#include <sys/mman.h>


#define procmodulesPath                      "/proc/modules"
#define lime_module_name                     "lime"
#define sysmoduleparameterspathPath          "/sys/module/%s/parameters/path"
#define sysmoduleparamtersformatPath         "/sys/module/%s/parameters/format"


#include "profile/appstatus.h"


// Detecting Hidden Modules, or rootkit
// 一些 memory dump 工具常以 loadable kernel module (lkm) 或rootkit的形式存在,
// 这里我们检测 /proc/modules的数量来判断是否有新的lkm在app执行后再载入.


// 检查 /proc/modules文件中loadable kernel modules (LKM)个数，如果有新增加的
// 模块则认为Android linux系统有hooking的行为，比如利用 LiME （lime.ko) 做 memory dump。

// 具体做法，初始化的时候保存载入模块数目。在随后的runtime 检查中比较。

// 面对这种瞬时memory dump/snapshot攻击，安全程序应该在使用完memory buffer后立即重新加密。

/*
 * identifying newly loaded kernel module during app runtime
 * - While app first run, retrieve the amount of loaded kernel modules and store it
 * - Next, during the app runtime, retrieve the latest amount of loaded kernel modules and
 *   compare with stored value
 * - If the new kernel module is spotted, check the kernel module's parameter (e.g. 'path', 'format')
 *   which essential for LIME memory dump tool
 *  refer to: https://github.com/504ensicslabs/lime#present
 */



int check_kernel_modules()
{
    static int initial_km_count = 0;

    int status = 0;  // normal

    FILE *fp = fopen(procmodulesPath, "r");
    if (fp == NULL)
    {
        printf("unable to read %s.\n", procmodulesPath);
        return 0;
    }

    int km_counter = 0;
    char line[256];
    while (fgets(line, sizeof line, fp) != NULL)
    {
        km_counter = km_counter + 1;

        // read module name
        char module_name[64] = {0};
        sscanf(line, "%s %*d %*d %*s %*s %*x", module_name);

        printf("#%d: %s\n", km_counter, module_name);

        // full path for LiME parameter "path"
        char module_parameter_path[128] = {0};
        sprintf(module_parameter_path, sysmoduleparameterspathPath, module_name);

        // full path for LiME parameter "format"
        char module_parameter_format[128] = {0};
        sprintf(module_parameter_format, sysmoduleparamtersformatPath, module_name);

        // determine if two LiME specific files are existing in given path
        if (   (strstr(module_name, lime_module_name) != 0)   // name check may be loosed ?
                && (access(module_parameter_path, F_OK ) == 0)
                && (access(module_parameter_format, F_OK ) == 0))
        {
            printf("found LiME a memory dumping kernel module loaded!\n");
            status = 0x01;   // found LiME kernel module for memory dumping

            set_lkm_status(status);
        }
    }

    if (initial_km_count == 0) // for initialization
    {
        initial_km_count = km_counter;
    }
    else if (km_counter > initial_km_count)
    {
        // further check on those newly added kernel modules
        // the load order in /proc/modules is from top to bottom by default
        printf("kernel module count NOT matched: %d - %d\n", km_counter, initial_km_count);
        status = status | 0x02;

        set_lkm_status(status);
    }

    fclose(fp);
    return status;
}



// 以下代码未整理 from vguard: checks.c



//void self_wipe(const char* appID) {
//	char path[200];
//	char path3[200];
//	char perm[10];
//	char maps_path[30];
//	char line1[300];
//	char line2[300];
//	char line3[300];
//	char currentso_path[200];
//	long memloc_delete[300];
//	int memloc_count = 0;
//	long s, e;
//	long s1;
//	int memsize;
//	int count = 0, i = 0;
//	int selfwipe_start = 0;
//	int m = 0;
//	FILE* fp1 = NULL;
//	FILE* fp2 = NULL;
//	FILE* fp3 = NULL;
//
//	// obtain the file path of current .so file
//	sprintf(maps_path, "/proc/%d/maps", (int) getpid());
//	fp1 = fopen(maps_path, "r");
//	while (fgets(line1, sizeof line1, fp1) != NULL) {
//		if (strlen(line1) <= 50) {
//			continue;
//		}
//		sscanf(line1, "%lx-%*x %*s %*x %*x:%*x %*d %150s", &s1, currentso_path);
//		if ((long)JNI_OnLoad < s1) {
//			break;
//		}
//	}
//	fclose(fp1);
//
//	/* Wiping app memory
//	 * Stage 1 :
//	 * (a) wipe memory that loaded by this app itself
//	 *     (e.g. /data/data/vkey.android.helloworld/files/libsecurefileIO.so )
//	 * (b) wipe memory that loaded for dalvik relative usage (e.g. dev/ashmem/dalvik-LinearAlloc)
//	 * (c) wipe non-label memory region (larger than 65KB), which appeared after /system/lib/libdvm.so
//	 * 	   or /system/lib/libart.so
//	*/
//	fp2 = fopen(maps_path, "r");
//	while (fgets(line2, sizeof line2, fp2) != NULL) {
//		path[0] = '\0';
//		s = 0;
//		e = 0;
//		sscanf(line2, "%lx-%lx %4s %*x %*x:%*x %*d %s", &s, &e, perm, path);
//		memsize = e-s;
//		long *ptr;
//		ptr = &s;
//		LOGI("s %lx, e %lx, %s %s memsize %d",s, e, perm, path, memsize);
//
//		if ((strstr(path, appID) || strstr(path, "ashmem"))) {
//			if (strcmp(path, currentso_path)) {
//				if (!strstr(perm, "w")) {
//					if (mprotect((void *)*ptr, memsize, PROT_WRITE|PROT_READ|PROT_EXEC) == 0) {
//						LOGI("Stage 1 - wipe memory change to PROT_WRITE");
//						memset((void *)*ptr,0,memsize);
//					} else {
//						LOGI("Stage 1 - wipe memory NOT ABLE change to PROT_WRITE");
//						continue;
//					}
//				} else {
//					LOGI("Stage 1 - wipe memory 1");
//					memset((void *)*ptr,0,memsize);
//				}
//			}
//		}
//
//		/*
//		 * there are large memory region (>65KB) which is not labeled in the mapping, yet contain app data
//		 * our approach:
//		 * (a) locate the memory location of /system/lib/libdvm.so, then wipe the non-label
//		 * 		memory region which appeared after /system/lib/libdvm.so
//		 * (b) for the memory region which appeared before /system/lib/libdvm.so, we store the 'start'
//		 *     and 'end' address. We will wipe it in Stage 2 later
//		 * this approach would prevent the app being crashed during entire self_wipe process
//		 */
//		if (!strcmp(path, "/system/lib/libdvm.so") || !strcmp(path,"/system/lib/libart.so")) {
//			selfwipe_start = 1;
//		}
//		if (!strcmp(path, "") && (selfwipe_start == 1)) {
//			if (memsize > 65536) {
//				if (!strstr(perm, "w")) {
//					if (mprotect((void *)*ptr, memsize, PROT_WRITE|PROT_READ|PROT_EXEC) == 0) {
//							LOGI("Stage 1 - wipe memory RAW change to PROT_WRITE");
//							memset((void *)*ptr,0,memsize);
//					} else {
//							LOGI("Stage 1 - wipe memory RAW NOT ABLE change to PROT_WRITE");
//							continue;
//					}
//				} else {
//					LOGI("Stage 1 - wipe RAW memory 1");
//					memset((void *)*ptr,0,memsize);
//				}
//			}
//		} else if (!strcmp(path, "") && selfwipe_start == 0) {
//			if (memsize > 65536 && strstr(perm, "w")) {
//				memloc_delete[memloc_count] = s;
//				memloc_delete[memloc_count+1] = memsize;
//				memloc_count=memloc_count+2;
//			}
//		}
//
//	}
//	fclose(fp2);
//
//	 /* Stage 2 :
//	  * (a) wipe the non-label memory region (larger than 65KB), which appeared before
//	  *     /system/lib/libdvm.so or /system/lib/libart.so
//	  * (b) wipe the region that labeled as 'stack' or 'heap' explicitly
//	 */
//
//	for (m = 0; m < memloc_count; m=m+2) {
//		LOGI("Stage 2 - delete %d/%d recorded memory", m, (memloc_count+1)/2);
//		LOGI("Stage 2 - memloc_delete %lx, memsize %d",memloc_delete[m],(int)memloc_delete[m+1]);
//		memset((void *)memloc_delete[m],0,(int)memloc_delete[m+1]);
//	}
//
//	fp3 = fopen(maps_path, "r");
//	while (fgets(line3, sizeof line3, fp3) != NULL) {
//		path3[0] = '\0';
//		s = 0;
//		e = 0;
//		sscanf(line3, "%lx-%lx %4s %*x %*x:%*x %*d %s", &s, &e, perm, path3);
//		memsize = e-s;
//		long *ptr3;
//		ptr3 = &s;
//
//		if (strstr(line3, "heap") || strstr(line3, "stack")) {
//			if (strstr(perm, "w")) {
//				LOGI("Stage 2 - wipe heap/stack memory 2");
//				memset((void *)*ptr3,0,memsize);
//			}
//		}
//	}
//	fclose(fp3);
//	return;
//}


////////////////////////////////////////
// Java interfaces here
////////////////////////////////////////

/*
 * Check if new kernel module runtime loading
 * New kernel runtime loading is a essential condition for memory dump with LIME/Volatility
 * RETURN: 0 - new kernel module runtime loading NOT supported
 *		   2 - support new kernel module runtime loading
 */
//jint Java_com_example_fanghui_myapplication_NativeThreatsChecker_checkForEnv(
//		JNIEnv* env, jobject javaThis) {
//
//	int env_check = 0;
//	int env_supportkernelmodule = 0;
//
//	FILE *fp_module;
//	fp_module = fopen("/proc/modules", "r");
//	if (fp_module == NULL) {
//		env_supportkernelmodule = 0;
//	} else {
//		env_supportkernelmodule = 2;
//		fclose(fp_module);
//	}
//
//	env_check =   env_supportkernelmodule;
//	return env_check;
//}

/*
 * Check for memory dump activity by identifying newly loaded kernel module during app runtime
 * - While app first run, retrieve the amount of loaded kernel modules and store it
 * - Next, during the app runtime, retrieve the latest amount of loaded kernel modules and
 * 	 compare with stored value
 * - If the new kernel module is spotted, check the kernel module's parameter (e.g. 'path', 'format')
 *   which essential for LIME memory dump tool
 * RETURN: 0 - memory dump activitiy NOT detected
 * 		   self_wipe() - memory dump activity detected
 */
//jint Java_com_example_fanghui_myapplication_NativeThreatsChecker_checkForMemDump(
//		JNIEnv* env, jobject javaThis,jstring appID) {
//
//	char line[400];
//	char module_name[64] = "";
//	char module_parameter_path[50] = "";
//	char module_parameter_format[50] = "";
//	int kernelmodulecount = 0;
//	FILE *fp;
//	FILE *fp2;
//	int i;
//	fp = fopen("/proc/modules", "r");
//	if (fp == NULL) {
//		return 0;
//	}
//
//	while (fgets(line, sizeof line, fp) != NULL) {
//		kernelmodulecount = kernelmodulecount + 1;
//	}
//
//	if (kernelModuleFirstCheck == 0) {
//		kernelModuleFirstCheck_Count = kernelmodulecount;
//		kernelModuleFirstCheck = 1;
//	}
//
//	if (kernelmodulecount - kernelModuleFirstCheck_Count > 0) {
//		//LOGE("kernel module count NOT matched %d / %d", kernelModuleFirstCheck_Count, kernelmodulecount);
//		fclose(fp);
//		fp2 = fopen("/proc/modules", "r");
//		for (i = 0; i <  (kernelmodulecount - kernelModuleFirstCheck_Count); i++) {
//			fgets(line, sizeof line, fp2);
//			sscanf(line, "%s %*d %*d %*s %*s %*x", module_name);
//			sprintf(module_parameter_path, "/sys/module/%s/parameters/path", module_name);
//			sprintf(module_parameter_format, "/sys/module/%s/parameters/format", module_name);
//			if ((access(module_parameter_path, F_OK ) == 0) && (access(module_parameter_path, F_OK ) == 0)) {
//				//LOGE("LIME memorydump kernel module found. Start self-wipe memory");
//				const char *appIDStr = (*env)->GetStringUTFChars(env, appID, NULL);
//				if (appIDStr==NULL) {
//					return 0;
//				}
//				self_wipe(appIDStr);
//				/* mostly the app will sleep until here
//				 * the next 2 lines of code below this won't be reached,
//				 * as self_wipe() function will make our app sleep
//				 */
//				(*env)->ReleaseStringUTFChars(env, appID, appIDStr);
//				return 1;
//			} else {
//				//LOGE("LIME memorydump kernel module not found\n");
//
//			}
//		}
//		fclose(fp2);
//		return 0;
//	} else {
//		//LOGE("kernel module count matched %d / %d", kernelModuleFirstCheck_Count, kernelmodulecount);
//	}
//	fclose(fp);
//	return 0;
//}
