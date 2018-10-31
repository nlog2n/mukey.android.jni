#include <stdio.h>

#include "utility/log.h"
#include "utility/jnienv.h"



// In Java, you can print out the Java stack trace:
// for (StackTraceElement ste : Thread.currentThread().getStackTrace()) {
//    System.out.println(ste);
// }

// to callback java function for obtaining backtrace
#define StackTraceElement_str "java/lang/StackTraceElement"
#define Thread_str "java/lang/Thread"
#define currentThread_str "currentThread"
#define getStackTrace_str "getStackTrace"
#define getClassName_str "getClassName"
#define return_Thread_str "()Ljava/lang/Thread;"
#define return_StackTraceElement_str "()[Ljava/lang/StackTraceElement;"


// if Xposed has hooked onto functions and initiated a call to our generateTotp function,
// for example, the stack trace will include Xposed framework classes:
// ...
// at java.lang.reflect.Method.invokeNative(Native Method)
// at java.lang.reflect.Method.invoke(Method.java:515)
// at com.android.internal.os.ZygoteInit$MethodAndArgsCaller.run(ZygoteInit.java:777)
// at com.android.internal.os.ZygoteInit.main(ZygoteInit.java:593)
// at de.robv.android.xposed.XposedBridge.main(XposedBridge.java:126)    ===> xposed found
// at dalvik.system.NativeStart.main(Native Method)


// whitelist for java call trace, which I did not use for now
//   : to check if each call trace lies in this list

// blacklist for java call trace
//   :  any spot will lead to tampering detected
#define XPOSED_STRING_IN_JAVA_BACKTRACE "de.robv.android.xposed.XposedBridge"



// return:  0 -OK
int validate_java_calltrace()
{
    int i;
    JNIEnv *env = global_jvm_env;  // GetCurrenThreadJEnv();
    jclass StackTraceElementClass = (*env)->FindClass(env, StackTraceElement_str);
    if (StackTraceElementClass == NULL)
    {
        LOGE("not able to found stacktraceelementclass");
        return 0;
    }

    jclass ThreadClass = (*env)->FindClass(env, Thread_str);
    if (ThreadClass == NULL)
    {
        LOGE("not able to found ThreadClass");
        return 0;
    }

    jmethodID currentThreadID = (*env)->GetStaticMethodID(env, ThreadClass, currentThread_str, return_Thread_str);
    if (currentThreadID == NULL)
    {
        LOGE("not able to found currentThreadID");
        return 0;
    }

    jmethodID getStackTraceID = (*env)->GetMethodID(env, ThreadClass, getStackTrace_str, return_StackTraceElement_str);
    if (getStackTraceID == NULL)
    {
        LOGE("not able to found getStackTraceID");
        return 0;
    }

    jmethodID getClassNameID = (*env)->GetMethodID(env, StackTraceElementClass, getClassName_str, "()Ljava/lang/String;");
    if (getClassNameID == NULL)
    {
        LOGE("not able to found getClassNameID");
        return 0;
    }

    jobject currentThread =  (*env)->CallStaticObjectMethod(env, ThreadClass, currentThreadID);

    jarray callstackarr = (*env)->CallObjectMethod(env, currentThread, getStackTraceID);

    jsize len = (*env)->GetArrayLength(env, callstackarr);

    jobject ret;
    jstring jstr;
    for (i = 0; i < len; i++)
    {
        ret = (*env)->GetObjectArrayElement(env, callstackarr, i);
        jstr = (*env)->CallObjectMethod(env, ret, getClassNameID);
        const char * str = (*env)->GetStringUTFChars(env, jstr, 0);
        //LOGE("java calltrace #%d: %s", i, str);

        if ( strstr(str, XPOSED_STRING_IN_JAVA_BACKTRACE) != 0 )
        {
            LOGE("found xposed!!");
            return 1;
        }
    }
    return 0;
}