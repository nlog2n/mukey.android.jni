#ifndef __ANDROID_MAC_ADDR_H__
#define __ANDROID_MAC_ADDR_H__

#ifdef __cplusplus
extern "C" {
#endif


int get_wifi_mac_address(unsigned char *pMacAddr);
int check_wifi_mac_address(unsigned char *pMacAddr);


void print_mac_address(unsigned char* pMacAddr);
int is_valid_mac_addr(unsigned char* pMacAddr);
int is_equal_mac_addrs(unsigned char* pMacAddr1, unsigned char* pMacAddr2);


#ifdef __cplusplus
}
#endif

#endif
