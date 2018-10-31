#ifndef  __JNIENV_H__
#define  __JNIENV_H__

#include <jni.h>

#ifdef __cplusplus
extern "C" {
#endif



extern JavaVM *global_jvm;
extern JNIEnv *global_jvm_env;
extern jobject  global_Activity;

JNIEnv * GetCurrenThreadJEnv();
jclass  get_ContextClass();
jobject get_application_context();
jstring get_PackageCodePath();


#ifdef __cplusplus
}
#endif



#endif

