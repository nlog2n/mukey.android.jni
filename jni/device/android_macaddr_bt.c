#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <sys/system_properties.h>

#include "utility/log.h"

// 获取Bluetooth MAC address
// 首先读取系统属性  [ro.bt.bdaddr_path]: [/efs/bluetooth/bt_addr]
//     用来获取存地址的文件。
// 然后读该文件获取6子节MAC 地址
// root@zeroflte:/ # cat /efs/bluetooth/bt_addr                                   
// E0:99:71:F5:75:61
//  该方法不需要蓝牙打开,但需要root权限

#define ANDROID_OS_BLUETOOTH_ADDR_PATH   "ro.bt.bdaddr_path"


void print_mac_address(unsigned char* pMacAddr);
int read_mac_address_file(const char* filename, unsigned char *pMacAddr);

int get_bt_mac_address(unsigned char *pMacAddr)
{
    char btaddr_path[100] = {0};

    //  find the bluetooth address file from system properties
    int ret = __system_property_get(ANDROID_OS_BLUETOOTH_ADDR_PATH, btaddr_path);
    if (ret <= 0)
    {
        LOGE("warning: did not get bt address file.\n");
        return 0;  // fail
    }
    LOGI("bt addr file = %s\n", btaddr_path);

    if ( read_mac_address_file(btaddr_path, pMacAddr) )
    {
        LOGI("successfully get bt mac address from file.\n");
        //print_mac_address(pMacAddr);
        return 1;
    }

    return 0;
}

