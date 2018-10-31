#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "utility/jnienv.h"
#include "utility/log.h"

// 参考 AndroidDeviceID.java 中获取 Wifi MAC 代码
// 并参考 jni_imei 中 native JNI 实现.

// Java核心代码:
// 该方法不需要蓝牙打开,但需要权限:
// 在manifest.xml文件中要添加 <uses-permission android:name="android.permission.BLUETOOTH" />
    /*
    import android.bluetooth.BluetoothAdapter;

        private String getBluetoothAddr()
        {
            String szBTMAC = "";
            try {
                BluetoothAdapter m_BluetoothAdapter = null; // Local Bluetooth adapter
                m_BluetoothAdapter = BluetoothAdapter.getDefaultAdapter();
                szBTMAC = m_BluetoothAdapter.getAddress();
            }catch (Exception ex)
            {}

            return szBTMAC;
        }

    */


int get_jni_btmac(unsigned char* btmac)
{
    // 获取Java虚拟机运行环境
    JNIEnv *env = global_jvm_env;

	// 查找 BluetoothAdapter 类
    jclass cls_btAdapter = (*env)->FindClass(env, "android/bluetooth/BluetoothAdapter");
	if(cls_btAdapter == 0){
	   LOGE("[+] Error: FindClass error -BluetoothAdapter");
       return -1;
	}

    // 查找该类的静态方法 getDefaultAdapter
    jmethodID getDefaultAdapter = (*env)->GetStaticMethodID(env, cls_btAdapter, "getDefaultAdapter", "()Landroid/bluetooth/BluetoothAdapter;");
	if(getDefaultAdapter == 0){
	   LOGE("[+] Error: GetStaticMethodID error -getDefaultAdapter");
       return -1;
	}

    // 执行该方法获得缺省 bluetooth adapter
    jobject btAdapter = (*env)->CallStaticObjectMethod(env, cls_btAdapter, getDefaultAdapter);
    if ( btAdapter == 0) {
       LOGE("[+] Error: CallStaticObjectMethod -getDefaultAdapter");
       return -1;
    }

    // 获得方法 getAddress
    jmethodID  getAddress = (*env)->GetMethodID(env, cls_btAdapter, "getAddress", "()Ljava/lang/String;");

    // 执行方法
    jstring btmac_jstring = (*env)->CallObjectMethod(env, btAdapter, getAddress);
    jthrowable ex = (*env)->ExceptionOccurred(env);
    if (ex) {    // exception, mostly because don't have read_wifi permission
            // handle the exception, and clear it
            // (*env)->ExceptionDescribe(env);
            (*env)->ExceptionClear(env);
            return -1;
    }
    if ( btmac_jstring == 0)
    {
       LOGE("[+] Error: CallObjectMethod error -getAddress");
       return -1;
    }

    // 转换成 c string
    unsigned char btmac_cstr[32] = {0};
    jsize len = (*env)->GetStringUTFLength(env, btmac_jstring);
    (*env)->GetStringUTFRegion(env, btmac_jstring, 0, len, btmac_cstr);
    (*env)->DeleteLocalRef(env, btmac_jstring);

    // 转换成6bytes
    unsigned int iMac[6];
    if ( sscanf(btmac_cstr, "%x:%x:%x:%x:%x:%x", &iMac[0], &iMac[1], &iMac[2], &iMac[3], &iMac[4], &iMac[5]) < 0 )
    {
        LOGE("parse bt mac address string error: %s", btmac_cstr);
        return -1;
    }
    int i;
    for (i = 0; i < 6; i++)
    {
        btmac[i] = (unsigned char)iMac[i];
    }

	return 0;
}
