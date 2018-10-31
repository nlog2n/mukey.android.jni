#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "utility/jnienv.h"



// 我想在Native code中获取IMEI，但不想先在Java中实现，再JNI调用，而是想直接在Native中调用getDeviceId去获取.
// NativeCode中通过JNI反射调用Java层的代码，
// 这种做法会使得静态分析Java层代码的方法失效。

// Java核心代码:
// Imei = ((TelephonyManager) getSystemService(TELEPHONY_SERVICE)).getDeviceId();
// 加入权限
// 在manifest.xml文件中要添加 <uses-permission android:name="android.permission.READ_PHONE_STATE" />
//

//int get_jni_imei(char* imei); // 由于需要权限,仅用来检测一致,不用来生成device ID

int get_jni_imei(char* imei)
{
    //char imei[128] = {0};

    jthrowable ex;

    // 获取Java虚拟机运行环境
    JNIEnv *env = global_jvm_env;
    jobject mContext = global_Activity;

    if(mContext == 0){
	   printf("[+] Error: Context null\n");
       return -1;
	}

    jclass cls_context = get_ContextClass();
	if(cls_context == 0){
		printf("[+] Error: FindClass <android/content/Context> Error\n");
       return -1;
	}

    jmethodID getSystemService = (*env)->GetMethodID(env, cls_context, "getSystemService", "(Ljava/lang/String;)Ljava/lang/Object;");
	if(getSystemService == 0){
       return -1;
	}

    jfieldID TELEPHONY_SERVICE = (*env)->GetStaticFieldID(env, cls_context, "TELEPHONY_SERVICE", "Ljava/lang/String;");
	if(TELEPHONY_SERVICE == 0){
       return -1;
	}

    jstring str = (*env)->GetStaticObjectField(env, cls_context, TELEPHONY_SERVICE);
    jobject telephonymanager = (*env)->CallObjectMethod(env, mContext, getSystemService, str);
	if(telephonymanager == 0){
       return -1;
	}

    jclass cls_tm = (*env)->FindClass(env, "android/telephony/TelephonyManager");
	if(cls_tm == 0){
       return -1;
	}

    jmethodID getDeviceId = (*env)->GetMethodID(env, cls_tm, "getDeviceId", "()Ljava/lang/String;");
	if(getDeviceId == 0){
       return -1;
	}

    jstring imei_jstring = (*env)->CallObjectMethod(env, telephonymanager, getDeviceId);
    ex = (*env)->ExceptionOccurred(env);
    if (ex) {    // exception, mostly because don't have read_phone permission
            // handle the exception, and clear it
            // (*env)->ExceptionDescribe(env);
            (*env)->ExceptionClear(env);
            return -1;
    }
    if ( imei_jstring == 0)
    {
       return -1;
    }

    // 此时已找到 Imei in java string，可以直接返回给jni wrapper
    // 以下代码是转换成 c string
    /*
    const char* imei_tmp   = (*env)->GetStringUTFChars(env, imei_jstring, NULL);
    if (imei_tmp && strcmp(imei_tmp, "") != 0)
    {
            strcpy(imei, imei_tmp);
    }
    (*env)->ReleaseStringUTFChars(env, imei_jstring, imei_tmp);
    */


    // 另一种方法
    jsize len = (*env)->GetStringUTFLength(env, imei_jstring);    
    (*env)->GetStringUTFRegion(env, imei_jstring, 0, len, imei);
    (*env)->DeleteLocalRef(env, imei_jstring);

    // 如果有需要讲该 c string 转换成 jni string
    // jstring imei_jstring = (*env)->NewStringUTF(env, imei);

	return 0;
}





/*
// jstring Java_com_example_testjni_Manager_getDeviceIdJNI( JNIEnv* env, jobject thiz, jobject mContext ) 
JNIEXPORT jstring JNICALL Java_com_xxx_yyy_MainActivity_getIMEI(JNIEnv* env, jobject mContext)
{
    char imei[128] = {0};

    int ret =  get_jni_imei(env, mContext, imei);

    // 如果有需要讲该 c string 转换成 jni string
    jstring imei_jstring = (*env)->NewStringUTF(env, imei);
    return imei_jstring;
}
*/

