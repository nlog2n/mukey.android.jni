#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "utility/jnienv.h"
#include "utility/log.h"

// 参考 AndroidDeviceID.java 中获取 Wifi MAC 代码
// 并参考 jni_imei 中 native JNI 实现.
// Java核心代码:
// 该方法不需要Wifi打开,但需要权限: android.permission.ACCESS_WIFI_STATE
// 加入权限
// 在manifest.xml文件中要添加 <uses-permission android:name="android.permission.ACCESS_WIFI_STATE" />
    /*
    private  String getMacAddress() {
        String macAddr = "";
        try {
            WifiManager manager = (WifiManager) myContext.getSystemService(Context.WIFI_SERVICE);
            WifiInfo info = manager.getConnectionInfo();
            macAddr = info.getMacAddress();   // .replace(":", "");
            //macAddr.getBytes();    // getBytes("UTF8");
        }
        catch (Exception ex)
        {
        }
        return macAddr;
    }
    */

int get_jni_wifimac(unsigned char* wifimac)
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

    jmethodID getSystemService = (*env)->GetMethodID(env, cls_context, "getSystemService", "(Ljava/lang/String;)Ljava/lang/Object;");
	if(getSystemService == 0){
	   LOGE("[+] Error: GetMethodID error -getSystemService");
       return -1;
	}

    jfieldID WIFI_SERVICE = (*env)->GetStaticFieldID(env, cls_context, "WIFI_SERVICE", "Ljava/lang/String;");
	if(WIFI_SERVICE == 0){
       LOGE("[+] Error: GetStaticFieldID error -WIFI_SERVICE");
       return -1;
	}

    jstring str = (*env)->GetStaticObjectField(env, cls_context, WIFI_SERVICE);
    if (str == 0) {
       LOGE("[+] Error: GetStaticObjectField error -WIFI_SERVICE");
       return -1;
    }
    jobject wifimanager = (*env)->CallObjectMethod(env, mContext, getSystemService, str);
	if(wifimanager == 0){
	   LOGE("[+] Error: CallObjectMethod error -getSystemService");
       return -1;
	}

    jclass cls_tm = (*env)->FindClass(env, "android/net/wifi/WifiManager");
	if(cls_tm == 0){
	   LOGE("[+] Error: FindClass error -WifiManager");
       return -1;
	}

    jmethodID getConnectionInfo = (*env)->GetMethodID(env, cls_tm, "getConnectionInfo", "()Landroid/net/wifi/WifiInfo;");
	if(getConnectionInfo == 0){
	   LOGE("[+] Error: GetMethodID error -getConnectionInfo");
       return -1;
	}

    // WifiInfo
    jobject wifiinfo = (*env)->CallObjectMethod(env, wifimanager, getConnectionInfo);
    if ( wifiinfo == 0 ){
       LOGE("[+] Error: CallObjectMethod error -getConnectionInfo");
       return -1;
    }

    jclass cls_WifiInfo = (*env)->FindClass(env, "android/net/wifi/WifiInfo");
    if ( cls_WifiInfo == 0 ) {
       LOGE("[+] Error: FindClass error -WifiInfo");
       return -1;
    }

    jmethodID getMacAddress = (*env)->GetMethodID(env, cls_WifiInfo, "getMacAddress", "()Ljava/lang/String;");
    if (getMacAddress == 0) {
       LOGE("[+] Error: GetMethodID error -getMacAddress");
       return -1;
    }

    jstring wifimac_jstring = (*env)->CallObjectMethod(env, wifiinfo, getMacAddress);
    jthrowable ex = (*env)->ExceptionOccurred(env);
    if (ex) {    // exception, mostly because don't have read_wifi permission
            // handle the exception, and clear it
            // (*env)->ExceptionDescribe(env);
            (*env)->ExceptionClear(env);
            return -1;
    }
    if ( wifimac_jstring == 0)
    {
       LOGE("[+] Error: CallObjectMethod error -getMacAddress");
       // strange thing for HTC phone: 最初一次取不到值,但把wifi打开然后又关闭后该函数可以取到值.
       return -1;
    }

    // 转换成 c string
    unsigned char wifimac_cstr[32] = {0};
    jsize len = (*env)->GetStringUTFLength(env, wifimac_jstring);
    (*env)->GetStringUTFRegion(env, wifimac_jstring, 0, len, wifimac_cstr);
    (*env)->DeleteLocalRef(env, wifimac_jstring);

    // 转换成6bytes
    unsigned int iMac[6];
    if ( sscanf(wifimac_cstr, "%x:%x:%x:%x:%x:%x", &iMac[0], &iMac[1], &iMac[2], &iMac[3], &iMac[4], &iMac[5]) < 0 )
    {
        LOGE("parse wifi mac address string error: %s", wifimac_cstr);
        return -1;
    }
    int i;
    for (i = 0; i < 6; i++)
    {
        wifimac[i] = (unsigned char)iMac[i];
    }

	return 0;
}
