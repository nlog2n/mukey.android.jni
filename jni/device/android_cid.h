#ifndef __ANDROID_CID_H__
#define __ANDROID_CID_H__

#ifdef __cplusplus
extern "C" {
#endif

// SD Card number  (CID, 16 bytes, shown as 32-size string
// read android SD Card Identification (CID) 
// return:  0 fail, 1 success
int get_android_cid(char* buffer);


#ifdef __cplusplus
}
#endif

#endif