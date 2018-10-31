#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "deviceid.h"
#include "utility/property_get.h"
#include "utility/log.h"


// check whether the device is rooted or an emulator, based on system properties
// return:   0-OK,  1-rooted, 2-emulator
int check_system_properties()
{
	// for system property get, also refer to my android_sys_info.c,h.
	char value[PROP_VALUE_MAX] = {0};  // as string
	int len;

	// 属性 ro.secure 为 0 , 判断是rooted运行
	len = __system_property_get("ro.secure", value);
	if ( len > 0 && strcmp(value, "0") == 0)
	{
		//  not running in secure mode
		//   regarded as for rooted device
		LOGE("ro.secure = 0 --> rooted");
		return 1;
	}

	len = __system_property_get("service.adb.root", value);
	if (len > 0 && strcmp(value, "1") == 0)
	{
		// rooted
		LOGE("service.adb.root = 1  --> rooted");
		return 1;
	}

	// 属性 ro.kernel.qemu 为 1 , 表示是模拟器运行
	len = __system_property_get("ro.kernel.qemu", value);
	if (len > 0 && strcmp(value, "1") == 0)
	{
		//  in emulator
		LOGE("ro.kernel.qemu = 1 --> in emulator");
		return 2;
	}

	len = __system_property_get("ro.debuggable", value);
	if (len > 0 && strcmp(value, "1") == 0)
	{
		// for emulator
		LOGE("ro.debuggable = 1 --> in emulator");
		return 2;
	}

	// 查看系统/system/build.prop信息中的Build.TAGS是否为test-keys(for AOSP，第三方ROM无效)
	// emulator:  ro.build.tags=test-keys
	// real:      ro.build.tags=release-keys
	len = __system_property_get("ro.build.tags", value);
	if (len > 0 && strcmp(value, "test-keys") == 0)
	{
		// for emulator
		LOGE("ro.build.tags  = test-keys --> in emulator");
		return 2;
	}

    // 查看属性是否包含serial no. 如无则认为在emulator中运行
	len = __system_property_get("ro.serialno", value);
	// 	if ( device->sysInfo.SerialNo.len == 0 )
	if ( len == 0 )
	{
		// empty, must be in emulator
		LOGE("ro.serialno  = (empty)  --> in emulator");
		return 2;
	}

	return 0;
}


// detect emulator by checking system property info
// return:   1 - emulator, 0 - OK
int detect_emulator(android_device_info* device)
{
	int is_in_emulator = 0;
	int is_imei_inconsistent = 0;
	int is_wifi_mac_inconsistent = 0;
	int is_bt_mac_inconsistent = 0;

    // check system properties
	is_in_emulator = (check_system_properties() == 2);


    // check if two IMEIs are same
	char imei[128] = {0};
    int status =  get_jni_imei(imei); // 由于需要权限,该函数仅用来检测一致,不用来生成device ID
    if ( status == 0 )   // success
    {
       LOGI("IMEI (jni) = %s", imei);
       // compare only if IMEI from java is obtained (with permission allowed)
       if ( strlen(device->imei) > 0 )
       {
           if ( strcmp(device->imei, imei) != 0 )
           {
               LOGE("Warning: inconsistent IMEIs: %s, %s", imei, device->imei);
               is_imei_inconsistent = 1;
           }
       }
    }

    // check if two Wifi MAC addresses are same
    unsigned char wifiMac2[6] = {0};
    status = check_wifi_mac_address(wifiMac2);
    if (status)
    {
       if ( is_valid_mac_addr(wifiMac2)  &&  is_valid_mac_addr(device->wifimacAddr) )
       {
       if ( !is_equal_mac_addrs(wifiMac2, device->wifimacAddr) )
       {
          LOGE("Warning: inconsistent Wifi MACs:");
          print_mac_address(wifiMac2);
          print_mac_address(device->wifimacAddr);
          is_wifi_mac_inconsistent = 1;
       }

       }
    }
    // further check wifi mac against jni result
    unsigned char wifiMac3[6] = {0};
    status = get_jni_wifimac(wifiMac3);
    if (status == 0)
    {
       if ( is_valid_mac_addr(device->wifimacAddr)  && is_valid_mac_addr(wifiMac3) )
       {
           if ( !is_equal_mac_addrs(device->wifimacAddr, wifiMac3) )
           {
                         LOGE("Warning: inconsistent Wifi MACs:");
                         print_mac_address(wifiMac3);
                         print_mac_address(device->wifimacAddr);
                         is_wifi_mac_inconsistent = 1;
           }
       }
    }

    // check if two bt MAC addresses are same
    unsigned char btMac2[6] = {0};
    status = get_jni_btmac(btMac2);
    if ( status == 0 )
    {
       LOGI("Bluetooth MAC(jni):"); print_mac_address(btMac2);
       if ( is_valid_mac_addr(device->btmacAddr)  && is_valid_mac_addr(btMac2) )
       {
           if ( !is_equal_mac_addrs(device->btmacAddr, btMac2) )
           {
                         LOGE("Warning: inconsistent Wifi MACs:");
                         print_mac_address(btMac2);
                         print_mac_address(device->btmacAddr);
                         is_bt_mac_inconsistent = 1;
           }
       }
    }

	return (is_in_emulator || is_imei_inconsistent || is_wifi_mac_inconsistent || is_bt_mac_inconsistent);
}
