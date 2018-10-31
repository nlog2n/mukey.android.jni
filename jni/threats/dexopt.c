#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/file.h>
#include <sys/wait.h>
#include <sys/system_properties.h>

#include "utility/log.h"


// dexopt 是Android VM 带的一个命令行工具,用来verify and optimize dex files
// 它的输出是odex fle (optimized dex),还是交由dalvik vm执行，而不是art.
// 这里我们利用dexopt提供的验证功能来判断classes.dex文件是否被篡改.

// 详情请参考 security.android/dex_signature/rundexopt.c 中说明.



// 对指定 APK文件执行dexopt in child process
// fork process在 child process中执行 execl dexopt
// input:  zipName  - APK
//         odexName - output odex file name
// return:  none
void execDexOpt(const char* zipName, const char* odexName, int zipFd, int odexFd)
{
    // Step 1: 执行文件路径: /system/bin/dexopt 程序。
    // 在5.0及以上版本的设备上，你可能已经再也找不到dexopt了，取而代之的是dex2oat

    // find dexopt executable; this exists for simulator compatibility
    const char* androidRoot = getenv("ANDROID_ROOT");
    if (androidRoot == NULL)
    {
        androidRoot = "/system";
    }
    static const char* kDexOptBin = "/bin/dexopt";
    char execFile[100] = {0};
    sprintf(execFile, "%s%s", androidRoot, kDexOptBin);


    // Step 2: pull optional configuration tweaks out of properties
    char dexoptFlags[100] = {0};
    int len = __system_property_get("dalvik.vm.dexopt-flags", dexoptFlags);


    // Step 3:  args for files
    static const int kMaxIntLen = 12;
    char zipNum[kMaxIntLen];
    char odexNum[kMaxIntLen];
    sprintf(zipNum, "%d", zipFd);
    sprintf(odexNum, "%d", odexFd);

    // Step 4: 真正执行 execl dexopt
    // 注意dexopt要求6个参数，之前的dexoptWrapper.cpp缺少dexoptFlags参数(虽然为空)
    // 命令行格式参考: https://android.googlesource.com/platform/dalvik-snapshot/+/gingerbread/dexopt/OptMain.c
    // cmd, arg0=cmd, arg1="--zip", arg2=zipFd, arg3=odexFd, arg4=apkname, arg5=flags.
    LOGI("execl %s  zipFd=%s odexFd=%s zipfile=%s flags=%s\n", execFile, zipNum, odexNum, zipName, dexoptFlags);
    execl(execFile, execFile, "--zip", zipNum, odexNum, zipName, dexoptFlags, (char*) NULL);

    // only reach here when failed
    LOGE("exec dexopt error %s\n", strerror(errno));
    exit(63);
}



int runDexOpt(const char* zipName, const char* odexName)
{
    // 打开apk和创建odex文件
    /*
     * Open the zip archive and the odex file, creating the latter (and
     * failing if it already exists).  This must be done while we still
     * have sufficient privileges to read the source file and create a file
     * in the target directory.  The "classes.dex" file will be extracted.
     */
    int zipFd, odexFd;
    zipFd = open(zipName, O_RDONLY, 0);
    if (zipFd < 0) {
        LOGE("unable to open '%s': %s\n", zipName, strerror(errno));
        return -1;
    }

    odexFd = open(odexName, O_RDWR | O_CREAT | O_EXCL, 0644);
    if (odexFd < 0) {
        LOGE("unable to create '%s': %s\n", odexName, strerror(errno));
        close(zipFd);
        return -1;
    }


    LOGI("begin dexopt...");

    // fork a child process to execute dexopt
    pid_t pid = fork();
    if (pid < 0)
    {
        LOGE("fork process failed.");
        return -1;
    }

    if (pid == 0)
    {
        // in child process

        // lock the odex file
        if (flock(odexFd, LOCK_EX | LOCK_NB) != 0)
        {
            LOGE("unable to lock '%s': %s\n", odexName, strerror(errno));
            exit(62);
        }

        execDexOpt(zipName, odexName, zipFd, odexFd);  // does not return

        LOGE("child about to exit.\n");
        exit(67);                           // in case failed
    }
    else {
        // parent -- wait for child to finish
        LOGI("waiting for verify+opt, pid=%d\n", (int) pid);
        int status, oldStatus;
        pid_t gotPid;

        // Wait for the optimization process to finish.
        while (1) {
            gotPid = waitpid(pid, &status, 0);
            if (gotPid == -1 && errno == EINTR) {
                LOGI("waitpid interrupted, retrying\n");
            } else {
                break;
            }
        }

        // close files
        close(zipFd);
        close(odexFd);

        // get returned status
        if (gotPid != pid) {
            LOGE("waitpid failed: wanted %d, got %d: %s\n",
                   (int) pid, (int) gotPid, strerror(errno));
            return -1;
        }

        if (WIFEXITED(status) && WEXITSTATUS(status) == 0) {
            LOGI("end dexopt %s (success)\n", zipName);
            return 0;
        } else {
            LOGE("end dexopt %s (process failed, status=%d)\n", zipName, status);
            return -1;
        }
    }
}
