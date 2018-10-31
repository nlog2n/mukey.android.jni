#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <sys/ioctl.h>
#include <sys/socket.h>
#include <sys/types.h>

#include <net/if.h>
#include <netdb.h>


#include <sys/system_properties.h>

#include "utility/log.h"


// 获取Wi-Fi MAC address
// refer to:
// https://gist.github.com/duanhong169/9910162
//  getDeviceInfo.cpp for Android




void print_mac_address(unsigned char* pMacAddr)
{
    LOGI("MAC Address = %02x:%02x:%02x:%02x:%02x:%02x",
           (unsigned char) pMacAddr[0],
           (unsigned char) pMacAddr[1],
           (unsigned char) pMacAddr[2],
           (unsigned char) pMacAddr[3],
           (unsigned char) pMacAddr[4],
           (unsigned char) pMacAddr[5]);
}

int is_valid_mac_addr(unsigned char* pMacAddr)
{
   if ( pMacAddr[0] == 0 && pMacAddr[1] == 0 && pMacAddr[2] == 0 && pMacAddr[3] == 0 && pMacAddr[4] == 0 && pMacAddr[5] == 0)
       return 0;
   return 1;
}


int is_equal_mac_addrs(unsigned char* pMacAddr1, unsigned char* pMacAddr2)
{
   return ( pMacAddr1[0] == pMacAddr2[0] && pMacAddr1[1] == pMacAddr2[1]
         && pMacAddr1[2] == pMacAddr2[2] && pMacAddr1[3] == pMacAddr2[3]
         && pMacAddr1[4] == pMacAddr2[4] && pMacAddr1[5] == pMacAddr2[5]);
}


int read_mac_address_file(const char* filename, unsigned char *pMacAddr)
{
    char line[128] = {0};

    FILE* f = fopen(filename, "r");
    if ( f == NULL )
    {
        LOGE("file %s not found.\n", filename);
        return 0;
    }

    while (fgets(line, sizeof(line), f) != NULL)
    {
        unsigned int iMac[6];
        int i;
        if ( sscanf(line, "%x:%x:%x:%x:%x:%x", &iMac[0], &iMac[1], &iMac[2], &iMac[3], &iMac[4], &iMac[5]) < 0 )
        {
            fclose(f);
            return 0;
        }

        for (i = 0; i < 6; i++)
        {
            pMacAddr[i] = (unsigned char)iMac[i];
        }

    }

    fclose(f);
    return 1;
}


// 该方法必须Wifi open联网才可以取到
// mac address: 6 bytes
// return: 1 ok, 0 fail
int read_wifi_mac_address_live(const char* interface, unsigned char *pBuffer)
{
    struct ifreq *ifr;
    struct ifconf ifc;
    int sock, i;
    int numif;

    // find number of interfaces.
    memset(&ifc, 0, sizeof(ifc));
    ifc.ifc_ifcu.ifcu_req = NULL;
    ifc.ifc_len = 0;

    if ((sock = socket(PF_INET, SOCK_STREAM, 0)) < 0)
    {
        printf("cannot create socket.\n");
        return 0; //false;
    }

    if (ioctl(sock, SIOCGIFCONF, &ifc) < 0)
    {
        printf("ioctl socket error.\n");
        return 0; //false;
    }

    if ((ifr = (struct ifreq*) malloc(ifc.ifc_len)) == NULL)
    {
        printf("malloc ifreq error.\n");
        return 0; //false;
    }

    ifc.ifc_ifcu.ifcu_req = ifr;

    if (ioctl(sock, SIOCGIFCONF, &ifc) < 0)
    {
        printf("ioctl socket ifc error.\n");
        close(sock);
        free(ifr);
        return 0; //false;
    }

    // check each interface
    numif = ifc.ifc_len / sizeof(struct ifreq);
    for (i = 0; i < numif; i++)
    {
        struct ifreq *r = &ifr[i];
        struct sockaddr_in *sin = (struct sockaddr_in*)&r->ifr_addr;
        if (strcmp(r->ifr_name, "lo") == 0)
            continue; // skip loopback interface

        if (strcmp(r->ifr_name, interface) != 0)
            continue;  // must be specified for wifi interface

        // attempt get MAC address, could be wlan0, eth0,...
        if (ioctl(sock, SIOCGIFHWADDR, r) < 0)
        {
            continue;
        }

        // found
        memcpy(pBuffer, r->ifr_hwaddr.sa_data, 6);
        close(sock);
        free(ifr);
        return 1; //true;
    }

    printf("get mac address failed.\n");
    close(sock);
    free(ifr);
    return 0; //false;
}


// 该方法读取配置文件来获取Wifi MAC地址, 不需要联网或额外权限。
// 首先读 [wifi.interface]: [wlan0], 获得 interface name
// 然后读 "/sys/class/net/[interface]/address" as a text file
//   interface = wlan0, eth0, eth1, ...

// On Samsung Galaxy S6:
//  只有 wlan0, 无 eth0等。且wifi.interface 指向 wlan0
// 但 getprop 无 mac 地址显示

// On emulator:
//   既有eth0, 又有eth1, 无wlan0. 但wifi.interface 指向 eth1 !
//   可能用这种方式检测虚拟环境。
//   另有 getprop [wifi.interface.mac]: [08:00:27:72:03:19]

// note: // getprop ril.wifi_macaddr     // does not work!

#define  PATH_MAC_INTERFACE_ADDRESS   "/sys/class/net/%s/address"
//#define  PATH_MAC_INTERFACE_ADDR_LEN  "/sys/class/net/%s/addr_len"
#define  MAC_WIFI_INTERFACE  "wlan0"
//#define  MAC_ETH_INTERFACE   "eth0"

int read_wifi_mac_address_file(const char* interface, unsigned char *pMacAddr)
{
    char filename[128] = {0};

    memset(pMacAddr, 0, 6);

    sprintf(filename, PATH_MAC_INTERFACE_ADDRESS, interface);

    return read_mac_address_file(filename, pMacAddr);
}


int get_wifi_mac_address(unsigned char *pMacAddr)
{
    char wifi_interface[100] = {0};

    //  determine which net interface was used for WiFi.
    int ir = __system_property_get("wifi.interface", wifi_interface);
    if (ir <= 0)
    {
        printf("warning: did not get wifi interface. set to default %s\n", MAC_WIFI_INTERFACE);
        strcpy(wifi_interface, MAC_WIFI_INTERFACE);
    }
    printf("wifi interface = %s\n", wifi_interface);

    int success = 0;
    if ( read_wifi_mac_address_file(wifi_interface, pMacAddr) )
    {
        printf("successfully get wifi mac address from file.\n");
        //print_mac_address(pMacAddr);
        success = 1;
        //return 1;
    }

    return success;
}


// 从socket获取wifi mac address, 只用以检查一致性
int check_wifi_mac_address(unsigned char *pMacAddr)
{
    char wifi_interface[100] = {0};

    //  determine which net interface was used for WiFi.
    int ir = __system_property_get("wifi.interface", wifi_interface);
    if (ir <= 0)
    {
        printf("warning: did not get wifi interface. set to default %s\n", MAC_WIFI_INTERFACE);
        strcpy(wifi_interface, MAC_WIFI_INTERFACE);
    }
    printf("wifi interface = %s\n", wifi_interface);

    int success = 0;
    if ( read_wifi_mac_address_live(wifi_interface, pMacAddr) )
    {
        printf("successfully get wifi mac address from socket.\n");
        //print_mac_address(pMacAddr);
        success = 1;
    }

    return success;
}

