#ifndef  __ANDROID_JNI_ANDROID_ID_H__
#define  __ANDROID_JNI_ANDROID_ID_H__


#ifdef __cplusplus
extern "C" {
#endif

// 只能用java提供的函数, 为8子节, 可用来生成device fingerprint
int get_jni_androidid(unsigned char* androidId);

void print_android_id(unsigned char* androidId);

#ifdef __cplusplus
}
#endif


#endif