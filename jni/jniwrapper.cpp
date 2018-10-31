
#pragma GCC diagnostic ignored "-Wwrite-strings"

#include "api/muapi.h"
#include "jniwrapper.h"
#include "utility/log.h"
#include "threats/dexopt.h"

JavaVM *global_jvm = NULL;   // cache jvm
JNIEnv *global_jvm_env = NULL;    // cache jvm env
jobject global_Activity = NULL;  // cache Java MainActivity class object

extern int initial_tracer_pid;

extern int keepAlive;  // for gracefully terminating the thread




// 库载入时执行
jint JNI_OnLoad(JavaVM* vm, void* reserved)
{
    initial_tracer_pid  = getppid();

#if ENABLE_PTRACE_CHECK
    ptrace_check();  // put here to preempt other debuggers
#endif

#if ENABLE_THREAD_CHECK
    launch_thread_check();
#endif

  JNIEnv* env;
  if (vm->GetEnv(reinterpret_cast<void**>(&env), JNI_VERSION_1_6) != JNI_OK)
  {
    return -1;
  }

  // cache this global jvm in order to get jenv later
  global_jvm = vm;
  global_jvm_env = GetCurrenThreadJEnv();

  return JNI_VERSION_1_6;
}




// refer to: http://stackoverflow.com/questions/18302837/android-jni-ndk-application-context
//  global_Activity now was used by dex_signature.c
JNIEXPORT void JNICALL Java_com_nlog2n_mukey_MuServices_onCreateNative(JNIEnv *env, jobject thiz, jobject activityObject)
{
    global_Activity = env->NewGlobalRef(activityObject); // (thiz);
}


JNIEXPORT void JNICALL Java_com_nlog2n_mukey_MuServices_onDestroy(JNIEnv *, jobject obj)
{
    keepAlive = 0;
}

JNIEXPORT jint JNICALL Java_com_nlog2n_mukey_MuServices_GenOTP(JNIEnv *, jobject obj)
{
   int status = 0;

   //  generate OTP

   return status;
}


JNIEXPORT jstring JNICALL Java_com_nlog2n_mukey_MuThreats_printAppStatus(JNIEnv *env, jobject thiz)
{
  char result[6000] = {0};
  get_app_status(result, sizeof(result));

	return env->NewStringUTF(result);
}





// apk path 和 odex path 从 JNI传过来
JNIEXPORT jint JNICALL Java_com_nlog2n_mukey_DexOptCheck_runDexOpt(JNIEnv* env, jobject thiz, jstring apkPath, jstring dexOptPath)
{
	const char *apkPathStr = env->GetStringUTFChars(apkPath, NULL);
	if (NULL == apkPathStr)
		return 1;

	const char *dexOptPathStr = env->GetStringUTFChars(dexOptPath, NULL);
	if (NULL == dexOptPathStr)
		return 1;

	int status = runDexOpt(apkPathStr, dexOptPathStr);
	env->ReleaseStringUTFChars(apkPath, apkPathStr);
	env->ReleaseStringUTFChars(dexOptPath, dexOptPathStr);
	return status;
}
