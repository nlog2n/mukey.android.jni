#ifndef __ANDROID_SYS_INFO_H__
#define __ANDROID_SYS_INFO_H__

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

//#include <sys/system_properties.h>
#include "utility/property_get.h"

// These values are easy to obtain in native code via the interface defined
// in <sys/system_properties.h>, which has been around since the first NDK release.
// You just need to know the string identifier used on the Java side. Fortunately,
// with an open-source OS, we can find these easily. Here is a working example of retrieving the model name.

//
// Public codes are defined in http://developer.android.com/reference/java/lang/System.html#getProperty(java.lang.String).
// Codes below are defined in https://android.googlesource.com/platform/frameworks/base/+/refs/heads/master/core/java/android/os/Build.java.
// Items with * are intended for display to the end user.
//

// 只对那些与设备相关的，不会改变的信息感兴趣:
#define ANDROID_OS_BUILD_MODEL               "ro.product.model"                  // * The end-user-visible name for the end product..
#define ANDROID_OS_BUILD_MANUFACTURER        "ro.product.manufacturer"           // The manufacturer of the product/hardware.
#define ANDROID_OS_BUILD_BOARD               "ro.product.board"                  // The name of the underlying board, like "goldfish".
#define ANDROID_OS_BUILD_BRAND               "ro.product.brand"                  // The brand (e.g., carrier) the software is customized for, if any.
#define ANDROID_OS_BUILD_DEVICE              "ro.product.device"                 // The name of the industrial design.
#define ANDROID_OS_BUILD_PRODUCT             "ro.product.name"                   // The name of the overall product.
#define ANDROID_OS_BUILD_CPU_ABI             "ro.product.cpu.abi"                // The name of the instruction set (CPU type + ABI convention) of native code.
#define ANDROID_OS_BUILD_CPU_ABI2            "ro.product.cpu.abi2"               // The name of the second instruction set (CPU type + ABI convention) of native code.
#define ANDROID_DEVICE_RIL_PRODUCT_CODE      "ril.product_code"     // : [SM-G920IZKASTH]

#define ANDROID_OS_BUILD_HARDWARE            "ro.hardware"                       // The name of the hardware (from the kernel command line or /proc).
#define ANDROID_OS_BUILD_PLATFORM            "ro.board.platform"   // 这个也可以同样方式读取, like "ro.product.model"
#define ANDROID_DEVICE_ARCH                  "ro.arch"       // [ro.arch]: [exynos7420]
#define ANDROID_DEVICE_CHIPNAME              "ro.chipname"   //: [exynos7420]

#define ANDROID_OS_BUILD_PRODUCT_SERIALNO    "ro.product.serialno"   // seems empty
#define ANDROID_OS_BUILD_SERIALNO            "ro.serialno"          // cool
#define ANDROID_OS_BUILD_BOOT_SERIALNO       "ro.boot.serialno"     // cool, same
#define ANDROID_OS_RIL_SERIALNUMBER          "ril.serialnumber"    // 该序列号被Samsung列作serial number.

#define ANDROID_OS_BUILD_BOOT_BOOTLOADER     "ro.boot.bootloader"
#define ANDROID_OS_BUILD_BOOT_HARDWARE       "ro.boot.hardware"
#define ANDROID_OS_BUILD_BOOT_MODE           "ro.boot.mode"

#define ANDROID_OS_BUILD_BOOT_BASEBAND       "ro.boot.baseband"
#define ANDROID_DEVICE_GSM_VERSION_BASEBAND  "gsm.version.baseband"  //: [G920IDVU1AOE2]
#define ANDROID_DEVICE_BASEBAND              "ro.baseband"     //: [unknown]
#define ANDROID_DEVICE_MODEM_BOARD           "ril.modem.board"    //: [SHANNON333]
#define ANDROID_OS_BUILD_GSM_IMEI            "ro.gsm.imei"     // seems empty

//#define ANDROID_OS_BUILD_VERSION_RELEASE     "ro.build.version.release"          // * The user-visible version string. E.g., "1.0" or "3.4b5".
//#define ANDROID_OS_BUILD_VERSION_INCREMENTAL "ro.build.version.incremental"      // The internal value used by the underlying source control to represent this build.
//#define ANDROID_OS_BUILD_VERSION_CODENAME    "ro.build.version.codename"         // The current development codename, or the string "REL" if this is a release build.
//#define ANDROID_OS_BUILD_VERSION_SDK         "ro.build.version.sdk"              // The user-visible SDK version of the framework.
//#define ANDROID_OS_BUILD_DISPLAY             "ro.build.display.id"               // * A build ID string meant for displaying to the user.
//#define ANDROID_OS_BUILD_HOST                "ro.build.host"
//#define ANDROID_OS_BUILD_USER                "ro.build.user"
//#define ANDROID_OS_BUILD_ID                  "ro.build.id"                       // Either a changelist number, or a label like "M4-rc20".
//#define ANDROID_OS_BUILD_TYPE                "ro.build.type"                     // The type of build, like "user" or "eng".
//#define ANDROID_OS_BUILD_TAGS                "ro.build.tags"                     // Comma-separated tags describing the build, like "unsigned,debug".
//#define ANDROID_OS_BUILD_FINGERPRINT         "ro.build.fingerprint"              // A string that uniquely identifies this build. 'BRAND/PRODUCT/DEVICE:RELEASE/ID/VERSION.INCREMENTAL:TYPE/TAGS'.


// samsung specific
#define ANDROID_DEVICE_BOOT_WARRANTY_BIT     "ro.boot.warranty_bit"   //  knox only : [0]
#define ANDROID_DEVICE_WARRANTY_BIT          "ro.warranty_bit"        //  knox only : [0]
#define ANDROID_DEVICE_CSC_COUNTRY_CODE      "ro.csc.country_code"  // : [SINGAPORE]     // samsung country sales code
#define ANDROID_DEVICE_CSC_COUNTRY_CODE2     "ro.csc.countryiso_code"  //: [SG]





// 记录所有感兴趣的系统属性
typedef struct t_android_sys_info
{
    android_sys_info_item   SerialNo;         // "ro.serialno"
    android_sys_info_item   Imei;             // "ro.gsm.imei"
    android_sys_info_item   Platform;         // "ro.board.platform"
    android_sys_info_item   Hardware;         // "ro.hardware"

    android_sys_info_item   Model;            // "ro.product.model"
    android_sys_info_item   Manufacturer;     // "ro.product.manufacturer"
    android_sys_info_item   ProductSerialNo;  // "ro.product.serialno"
    android_sys_info_item   Name;             // "ro.product.name"
    android_sys_info_item   Board;            // "ro.product.board"
    android_sys_info_item   Brand;            // "ro.product.brand" 
    android_sys_info_item   Device;           // "ro.product.device" 
    android_sys_info_item   CPUAbi;           // "ro.product.cpu.abi" 
    android_sys_info_item   CPUAbi2;          // "ro.product.cpu.abi2" 

    android_sys_info_item   BootMode;         // "ro.boot.mode"
    android_sys_info_item   BootBaseBand;     // "ro.boot.baseband"
    android_sys_info_item   BootLoader;       // "ro.boot.bootloader"
    android_sys_info_item   BootHardware;     // "ro.boot.hardware"
    android_sys_info_item   BootSerialNo;     // "ro.boot.serialno" 

    android_sys_info_item   RilSerialNumber;  // "ril.serialnumber" 

    android_sys_info_item   Arch;             // "ro.arch" 
    android_sys_info_item   ChipName;         // "ro.chipname" 
    android_sys_info_item   CSCCountryCode;   // "ro.csc.country_code" 
    android_sys_info_item   CSCCountryCode2;  // "ro.csc.countryiso_code" 
    android_sys_info_item   ProductCode;      // "ril.product_code" 
    android_sys_info_item   GSMBaseBand;      // "gsm.version.baseband" 
    android_sys_info_item   BaseBand;         // "ro.baseband" 
    android_sys_info_item   ModemBoard;       // "ril.modem.board" 

    android_sys_info_item   KNOXWarrantyBit;   // "ro.boot.warranty_bit" 
    android_sys_info_item   KNOXWarrantyBit2;  // "ro.warranty_bit" 

}  android_sys_info;





#ifdef __cplusplus
extern "C" {
#endif


int get_system_properties(android_sys_info* info);

void print_system_properties(android_sys_info* info);


#ifdef __cplusplus
}
#endif


#endif