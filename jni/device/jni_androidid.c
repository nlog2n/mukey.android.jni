#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "utility/jnienv.h"
#include "utility/log.h"

// Android层提供的unique device id, 8 byte shown by java as len-16 hex string

// 参考 AndroidDeviceID.java 中获取 Wifi MAC 代码
// 并参考 jni_imei 中 native JNI 实现.
// Java核心代码:
// 该方法不需要权限， 该值可能雷同，或被其他应用改变
    /*
    import android.provider.Settings;

        private  String getDeviceId() {
            String identifier = "";
            try {
                identifier = Settings.Secure.getString(myContext.getContentResolver(), Settings.Secure.ANDROID_ID);
            }
            catch (Exception ex)
            {

            }
            return identifier;
        }
    */



int get_jni_androidid(unsigned char* androidId)
{
    // 获取Java虚拟机运行环境
    JNIEnv *env = global_jvm_env;
    jobject mContext = global_Activity;

    if(mContext == 0){
	   LOGE("[+] Error: Context null\n");
       return -1;
	}

    jclass cls_context = get_ContextClass();
	if(cls_context == 0){
		LOGE("[+] Error: FindClass <android/content/Context> Error\n");
       return -1;
	}

    // 在 context 类中查找 getContentResolver 方法
    jmethodID getContentResolver = (*env)->GetMethodID(env, cls_context, "getContentResolver", "()Landroid/content/ContentResolver;");
	if(getContentResolver == 0){
	   LOGE("[+] Error: GetMethodID error -getContentResolver");
       return -1;
	}

	// 执行 mContext.getContentResolver 方法
	jobject contentResolver = (*env)->CallObjectMethod(env, mContext, getContentResolver);

    // 查找 Settings.Secure 类
    jclass cls_SettingsSecure = (*env)->FindClass(env, "android/provider/Settings$Secure");
    if ( cls_SettingsSecure == 0 ) {
       LOGE("[+] Error: FindClass error -Settings.Secure");
       return -1;
    }

    jfieldID ANDROID_ID = (*env)->GetStaticFieldID(env, cls_SettingsSecure, "ANDROID_ID", "Ljava/lang/String;");
	if(ANDROID_ID == 0){
       LOGE("[+] Error: GetStaticFieldID error -ANDROID_ID");
       return -1;
	}
    jstring str = (*env)->GetStaticObjectField(env, cls_SettingsSecure, ANDROID_ID);
    if (str == 0) {
       LOGE("[+] Error: GetStaticObjectField error -ANDROID_ID");
       return -1;
    }

    // 查找类 Settings.Secure的getString方法
    jmethodID getString = (*env)->GetStaticMethodID(env, cls_SettingsSecure, "getString", "(Landroid/content/ContentResolver;Ljava/lang/String;)Ljava/lang/String;");
    if (getString == 0) {
       LOGE("[+] Error: GetStaticMethodID error -getString");
       return -1;
    }

    // 执行该方法
    jstring android_id_jstring = (*env)->CallStaticObjectMethod(env, cls_SettingsSecure, getString, contentResolver, str);
    jthrowable ex = (*env)->ExceptionOccurred(env);
    if (ex) {    // exception, mostly because don't have read_wifi permission
            // handle the exception, and clear it
            // (*env)->ExceptionDescribe(env);
            (*env)->ExceptionClear(env);
            return -1;
    }
    if ( android_id_jstring == 0)
    {
       LOGE("[+] Error: CallObjectMethod error -getString");
       return -1;
    }

    // 转换成 c string
    unsigned char android_id_cstr[32] = {0};
    jsize len = (*env)->GetStringUTFLength(env, android_id_jstring);
    (*env)->GetStringUTFRegion(env, android_id_jstring, 0, len, android_id_cstr);
    (*env)->DeleteLocalRef(env, android_id_jstring);

    // 转换成8bytes
    unsigned int iMac[8];
    if ( sscanf(android_id_cstr, "%02x%02x%02x%02x%02x%02x%02x%02x", &iMac[0], &iMac[1], &iMac[2], &iMac[3],
                &iMac[4], &iMac[5], &iMac[6], &iMac[7]) < 0 )
    {
        LOGE("parse android id string error: %s", android_id_cstr);
        return -1;
    }
    int i;
    for (i = 0; i < 8; i++)
    {
        androidId[i] = (unsigned char)iMac[i];
    }

	return 0;
}


void print_android_id(unsigned char* androidId)
{
    LOGI("Android ID = %02x%02x%02x%02x%02x%02x%02x%02x",
           (unsigned char) androidId[0],
           (unsigned char) androidId[1],
           (unsigned char) androidId[2],
           (unsigned char) androidId[3],
           (unsigned char) androidId[4],
           (unsigned char) androidId[5],
           (unsigned char) androidId[6],
           (unsigned char) androidId[7]);
}