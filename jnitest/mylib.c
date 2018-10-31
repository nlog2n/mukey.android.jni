#include <string.h>
#include <jni.h>

 

JNIEXPORT jint JNICALL
Java_org_eshock_jnitest_JNITest_plus( JNIEnv* env,
                                                  jobject thiz,
                                                  jint x,
                                                  jint y )
{
    return x + y;
}
