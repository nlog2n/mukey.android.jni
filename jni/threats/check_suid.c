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

// suid/sgid exploitations
//            to check if any program has capability to gain root privilege
// case 1:   file suid mode set,  uid = root     <--  executable for other/anyone
// case 2:   file sgid mode set,  gid = root     <--  executable for other/anyone
//
//            to check if my app has capability to gain root privilege
// case 3:   file suid mode set,  uid = root     <--  executable for group, app uid is in gid
// case 4:   file sgid mode set,  gid = root     <--  executable for group, app uid is in gid
// case 5:   app uid=root (unlikely)

// func: check file for setuid/setgid permission
// return: 0 -OK,  others - suspicious with suid root risk
// called by:  checkdir()
int check_sufile(const char* filepath)
{
	// get file stat
	struct stat st;
	if ( lstat(filepath, &st)  < 0 )
		return 0;   // cannot get file stat

	// white-list for skip
	if (strcmp(filepath, "/system/etc/resolv.conf") == 0)    // not sure the reason??
		return 0;   // filter out this file

	// check suid/sgid bits
	if (   ((st.st_mode & S_ISUID) && (st.st_uid == 0))
	        || ((st.st_mode & S_ISGID) && (st.st_gid == 0)))
	{
		LOGW("found suid/sgid file %s: mode=%lo, uid=%ld, gid=%ld", filepath, (unsigned long)st.st_mode, st.st_uid, st.st_gid);
		if (st.st_mode & S_IXOTH)  // executable for other users, so to gain root privilege
		{
			// found and collect result
			//LOGI("report positive this file:%s", filepath);
			return 1;   // that is for sure

		}
		else if (st.st_mode & S_IXGRP) // check if my app uid is in the same group with this suid/sgid file
		{
			int myuid = geteuid();  // get my effective user id, normally it is same as getuid()
			if ( myuid == st.st_uid )
			{
				return 2;
			}

			// check my groups
			int mygid = getegid(); // not specified on whether egid is in supplementary group list or not
			if ( mygid == st.st_gid )
			{
				return 3;
			}
			// for supplementary groups
			int numgroups, i;
			int numgroups_max =  sysconf(_SC_NGROUPS_MAX) + 1;
			gid_t groups[numgroups_max];
			numgroups = getgroups(numgroups_max, groups);  // get the number of supplementary group
			if ( numgroups != -1 )
			{
				for (i = 0; i < numgroups; i++)
				{
					if ( groups[i] == st.st_gid )  // i am member of this sgid group
					{
						return 4;
					}
				}
			}

		}
	}  // end if ( st_mode, st_uid, st_gid)

	return 0;
}



// 扫描结果存于此:
int n;    // number of suspicious files

struct node {
	char name[256];
	struct node *next;
};

struct node *files;   // list of files

// recursively check files system for suspicious files with root special permission
//    FILE *fp = _popen("find / -perm -2000 -o -perm -4000", "r");
void scanfs(char *curdir)
{
	DIR *dir = opendir(curdir);
	if (dir == NULL)  return;

	struct dirent *de;
	while ((de = readdir(dir)) != NULL)
	{
		// quit upon finding one  (or choose to continue checking?)
		/*
		if (n > 0) {
			break;
		}
		*/

		// create string name for child directory
		// + 2 because of the '/' and the terminating 0
		char *childdir = malloc(strlen(curdir) + strlen(de->d_name) + 2);
		if (strcmp(curdir, "/") == 0)
			sprintf(childdir, "/%s", de->d_name);
		else
			sprintf(childdir, "%s/%s", curdir, de->d_name);

		//repeat if child is directory
		if (de->d_type == DT_DIR)
		{
			//skip ".", "..", "mnt", "sdcard", "proc", "storage"
			if (strcmp(de->d_name, ".") && strcmp(de->d_name, "..")
			        && strcmp(de->d_name, "mnt") && strcmp(de->d_name, "sdcard")
			        && strcmp(de->d_name, "proc") && strcmp(de->d_name, "storage"))
			{
				scanfs(childdir);
			}
		}
		//check file for setuid/setgid permission
		else {
			int suspicious = check_sufile(childdir);
			if (suspicious)
			{
				// found and collect result
				LOGE("report positive this file:%s, errorcode=%d", childdir, suspicious);
				n++;
				struct node *tmp = (struct node *) malloc(sizeof(struct node));
				snprintf(tmp->name, 256, "%s", childdir);
				tmp->next = files;
				files = tmp;

			}

		}  // end if-else

		free(childdir);

	}  // end while

	closedir(dir);
}



// C interface function
// return:   0 -OK
//           n, number of found suid/sgid files
//           files, global variable
int check_su_files()
{
	int status = 0;

	// Check 1
	// check which directories are mounted as suid allowed or exec allowed
	/*
	FILE *file = fopen("/proc/mounts", "r");
	if (file != NULL)
	{
		char line[256];
		while (fgets(line, sizeof line, file) != NULL)
		{
			if ( strstr(line, "nosuid") == NULL && strstr(line, "noexec") == NULL)
			{
				// suspicious, should check this directory
				LOGW("mounted directory to check: %s", line);
			}
		}
		fclose(file);
	}
	*/


	// Check 2
	// init output result //reset
	if ( n > 0 && files != NULL )
	{
		// free memory
		struct node *next;
		while ( files != NULL )
		{
			next = files->next;
			free(files);
			files = next;
		}
	}
	n = 0;    // n=0 means no file found
	files = NULL;
	// scan file system and get result
	scanfs("/");

  // 获得结果
	if ( n > 0 )
	{
		set_root_jailbreak_status();
	}
	return n;
}


// JNI interface function
// #include <jni.h>
// JNIEXPORT jobjectArray Java_com_nlog2n_mukey_RootChecker_findSuidSgidFiles(JNIEnv *env, jobject thiz);
/*
jobjectArray Java_com_nlog2n_mukey_RootChecker_findSuidSgidFiles(JNIEnv *env, jobject thiz)
{
	check_su_files();
	if (n == 0)  // no found
	{
		return NULL;
	}

	// transform result into java string array
	jobjectArray result = (*env)->NewObjectArray(env, n, (*env)->FindClass(env, "java/lang/String"), NULL);
	struct node *t, *next;
	int i = 0;
	for (i = 0, t = files; t != NULL; i++, t = next)
	{
		jstring tmp = (*env)->NewStringUTF(env, t->name);
		(*env)->SetObjectArrayElement(env, result, i, tmp);
		next = t->next;
		free(t);
	}
	return result;
}
*/
