#include <stdlib.h>
#include <stdio.h>
#include <string.h>

// android下如何读取mmc、sd卡cid等信息 
// 这里只取MMC卡信息。（Samsung Galaxy S6无该文件)

// 软件需要绑定SD卡，这时候需要一个唯一标识码UUID，一般我们会获取SD卡的CID。
// 通过读取cat /sys/block/mmcblkx/device/cid 来获取，
//  一般过程是先获取device/type来确定设备类型，常见类型mmc，sd，然后再读取device/cid来获取设备唯一标识码。
//
// (MMC/Nand)
#define  MMC_CID_PATH  "/sys/block/mmcblk0/device/cid" 

// (插入的microSD卡)  
#define  SD_CID_PATH   "/sys/block/mmcblk1/device/cid"


// CID格式参考： 
// http://blog.csdn.net/lyfadd85/article/details/9112439
// http://zhiwei.li/text/2015/08/16/android%E7%A1%AC%E4%BB%B6id/ 
// https://www.kernel.org/doc/Documentation/mmc/mmc-dev-attrs.txt

// CID for HTC phone
// CID =45010053454d303847c522486079b00a


// SD Card number  (CID, 16 bytes, shown as 32-size string
// read android SD Card Identification (CID) 
// return:  0 fail, 1 success
int get_android_cid(char* buffer)
{
    char line[256]={0};
    char cid[33]={0};

    FILE *f = fopen(MMC_CID_PATH, "r");
    if (f == NULL)
    {
        printf("cid file not found: %s\n", MMC_CID_PATH);
        return 0; 
    }

    while (fgets(line, sizeof line, f) != NULL)
    {
        sscanf(line, "%s", cid);   // one cid per line
    }
    fclose(f);

    int length = strlen(cid);
    if (length == 0)
    {
        length = 32;
        memset(cid, '0', length);
    }

    cid[length] = 0;  //  shown as a size-32 string
    memcpy(buffer, cid, length);
    //printf("cid=%s", cid);
    return 1;
}

