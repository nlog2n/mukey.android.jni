#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "utility/log.h"
#include "deviceid.h"


void print_android_device_info(android_device_info* device)
{
    print_android_id(device->androidID);
    print_system_properties(&(device->sysInfo));
    print_android_cpuinfo(&(device->cpuInfo));
    LOGI("IMEI =%s", device->imei);
    LOGI("Wifi      "); print_mac_address(device->wifimacAddr);
    LOGI("Bluetooth "); print_mac_address(device->btmacAddr);
    LOGI("CID =%s", device->cid);
}


int read_android_device_info(android_device_info* device)
{
	int success;

	memset(device, 0, sizeof(android_device_info));

	// get android id
	success = get_jni_androidid(device->androidID);

	// get system properties
	success = get_system_properties(&(device->sysInfo));

	// get cpu info
	success = get_android_cpuinfo(&(device->cpuInfo));

	// get imei
	success = get_android_imei(device->imei);

	// get wifi mac address
	success = get_wifi_mac_address(device->wifimacAddr);

    // get bluetooth mac address
	success = get_bt_mac_address(device->btmacAddr);

	// get sd card number
	success = get_android_cid(device->cid);

	return success;
}


// compute hash for whole device information
unsigned int generate_device_id(android_device_info* device)
{
	int i;
	unsigned int hash = 0;
	unsigned char* ptr = (unsigned char*)device;
	for(i=0; i < sizeof(android_device_info); i++)
	{
		hash +=  *(ptr+i);
	}
	LOGI("\n====>android device hash: %d\n", hash);
	return hash;
}



#ifdef  TEST_BACKTRACE

int main()
{
	android_device_info device;
	read_android_device_info(&device);
	print_android_device_info(&device);
	generate_device_id(&device);
	detect_emulator(&device);
	return 0;
}

#endif