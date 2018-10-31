#ifndef __ANDROID_IMEI_H__
#define __ANDROID_IMEI_H__


#ifdef __cplusplus
extern "C" {
#endif


// Android设备ID目前没有好的native code可以得到，但可以通过java method取到.
int get_android_imei(char* buffer);


#ifdef __cplusplus
}
#endif

#endif