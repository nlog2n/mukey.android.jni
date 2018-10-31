#ifndef  __ANDROID_JNI_BT_MAC_H__
#define  __ANDROID_JNI_BT_MAC_H__


#ifdef __cplusplus
extern "C" {
#endif


int get_jni_btmac(unsigned char* btmac); // 由于需要权限,仅用来检测一致,不用来生成device ID



#ifdef __cplusplus
}
#endif


#endif