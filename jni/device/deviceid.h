#ifndef  __ANDROID_DEVICE_ID_H__
#define  __ANDROID_DEVICE_ID_H__


#include "android_sys_info.h"
#include "android_cpuinfo.h"
#include "android_imei.h"
#include "android_macaddr_wifi.h"
#include "android_macaddr_bt.h"
#include "android_cid.h"
#include "jni_imei.h"
#include "jni_wifimac.h"
#include "jni_androidid.h"
#include "jni_btmac.h"


typedef struct t_android_device_info
{
    unsigned char     androidID[8];    // as 8 bytes
	android_sys_info  sysInfo;         // as struct
	cpu_info          cpuInfo;         // as struct
	char              imei[16];        // as string of size 15
	unsigned char     wifimacAddr[6];  // as 6 bytes
	unsigned char     btmacAddr[6];    // as 6 bytes
	char              cid[33];         // as string of size 32
}  android_device_info;



#ifdef __cplusplus
extern "C" {
#endif

void print_android_device_info(android_device_info* device);
int read_android_device_info(android_device_info* device);
unsigned int generate_device_id(android_device_info* device);
int detect_emulator(android_device_info* device);

#ifdef __cplusplus
}
#endif

#endif