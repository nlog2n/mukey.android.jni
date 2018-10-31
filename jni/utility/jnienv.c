#include <stdio.h>
#include <pthread.h>

#include "utility/log.h"
#include "jnienv.h"


// The callback  should be in VM thread which loads the library.
// The JNI implementation maintains a JNIEnv per thread, and puts the pointer in thread-local storage.
// It is only initialized for native threads which are attached to the VM.
// You need to call AttachCurrentThread() (or more probably AttachCurrentThreadAsDaemon()) inside
// the callback to get a JNIEnv pointer valid for that thread.

// refer to: http://stackoverflow.com/questions/5991615/unable-to-get-jnienv-value-in-arbitrary-context

// https://developer.vuforia.com/library/articles/Solution/How-To-Communicate-Between-Java-and-C-using-the-JNI

// http://stackoverflow.com/questions/12900695/how-to-obtain-jni-interface-pointer-jnienv-for-asynchronous-calls


//this will be called when attached thread exit
static void OnJavaDetach(void* arg)
{
    (*global_jvm)->DetachCurrentThread(global_jvm);

    //release thread key
    pthread_key_t *javaDetach = (pthread_key_t*)arg;

    pthread_setspecific(*javaDetach, NULL);
    pthread_key_delete(*javaDetach);

    free(javaDetach);
}


// attach VM context to current thread, which may not be the same thread as JNI_Onload
// Once attached the method can be called.
static JNIEnv * AttachCurrenThreadJEnv()
{
    JNIEnv *env;
    if ((*global_jvm)->AttachCurrentThread(global_jvm, &env, NULL)!= JNI_OK)
        return NULL;

    //create thread destructor
    pthread_key_t *javaDetach = malloc( sizeof(pthread_key_t));
    pthread_key_create(javaDetach, OnJavaDetach);
    //set thread key value so OnJavaDetach() will be called when attached thread exit
    pthread_setspecific(*javaDetach, javaDetach);

    return env;
}

JNIEnv * GetCurrenThreadJEnv()
{
    JNIEnv *env;
    // double check it's all ok
    jint re = (*global_jvm)->GetEnv(global_jvm, (void**)&env, JNI_VERSION_1_6);
    if(re == JNI_EDETACHED)
    {
        env = AttachCurrenThreadJEnv();
    }
    else if (re != JNI_OK)
    {
        env = NULL;
    }

    return env;
}


//#define ctx_str "ctx"
//#define ctx_type_str "Landroid/content/Context;"
#define ctx_class_str "android/content/Context"

#define getApplicationContext_str "getApplicationContext"
#define getApplicationContext_args_str "()Landroid/content/Context;"

#define getPackageCodePath_str "getPackageCodePath"
#define getPackageCodePath_args_str "()Ljava/lang/String;"

jclass  get_ContextClass()
{
        JNIEnv *env = global_jvm_env; //GetCurrenThreadJEnv();
        if ( env == NULL ) return 0;

        jclass Contextclass = (*env)->FindClass(env,ctx_class_str);
        return Contextclass;
}


jobject get_application_context()
{
    JNIEnv *env = global_jvm_env;  //GetCurrenThreadJEnv();
    if ( env == NULL )  return NULL;

    //interpret "Context context = getApplicationContext();"
    jclass Contextclass = (*env)->FindClass(env,ctx_class_str);
    if(Contextclass == NULL)
    {
        LOGE("FindClass error: android/content/Context");
        return NULL;
    }
    jmethodID getApplicationContextID = (*env)->GetMethodID(env, Contextclass, getApplicationContext_str, getApplicationContext_args_str);
    if(getApplicationContextID == NULL)
    {
        LOGE("GetMethodID error: getApplicationContext");
        return NULL;
    }

    jobject AppContext = (*env)->CallObjectMethod(env,global_Activity,getApplicationContextID);
    return AppContext;

    // 或者直接返回 global_Activity ??
    // get java MainActivity class
    // jclass main_activity_class = (*env)->GetObjectClass(env, global_Activity);
}


jstring get_PackageCodePath()
{
    JNIEnv *env = global_jvm_env; //GetCurrenThreadJEnv();

    // get java MainActivity class
    //jclass main_activity_class = (*env)->GetObjectClass(env, global_Activity);

    //interpret "String path = getApplicationContext().getPackageCodePath();"
    jclass Contextclass = (*env)->FindClass(env,ctx_class_str);
    if(Contextclass == NULL)
    {
        return 0;
    }
    jmethodID getApplicationContextID = (*env)->GetMethodID(env, Contextclass, getApplicationContext_str, getApplicationContext_args_str);
    if(getApplicationContextID == NULL)
    {
        return 0;
    }

    jobject AppContext =   (*env)->CallObjectMethod(env,global_Activity,getApplicationContextID);

    // getPackageCodePath
    jmethodID getPackageCodePath = (*env)->GetMethodID(env, Contextclass, getPackageCodePath_str, getPackageCodePath_args_str);
    if(getPackageCodePath == NULL)
    {
        LOGE("GetMethodID error: getPackageCodePath");
        return 0;
    }

    jstring path = (*env)->CallObjectMethod(env,AppContext,getPackageCodePath);
    return path;
}