#include <dlfcn.h>
//#include <unistd.h>
//#include <fcntl.h>

#include "android_sys_info.h"
#include "utility/log.h"

#define printf LOGI


int get_system_properties(android_sys_info* info)
{
    // reset
    if ( !info )  return 0;
    memset(info, 0, sizeof(android_sys_info));

    _get_system_property(ANDROID_OS_BUILD_MODEL, &(info->Model));
    _get_system_property(ANDROID_OS_BUILD_SERIALNO, &(info->SerialNo));
    _get_system_property(ANDROID_OS_BUILD_GSM_IMEI, &(info->Imei));
    _get_system_property(ANDROID_OS_BUILD_MANUFACTURER, &(info->Manufacturer));
    _get_system_property(ANDROID_OS_BUILD_PRODUCT_SERIALNO, &(info->ProductSerialNo));
    _get_system_property(ANDROID_OS_BUILD_PRODUCT, &(info->Name));
    _get_system_property(ANDROID_OS_BUILD_PLATFORM, &(info->Platform));
    _get_system_property(ANDROID_OS_BUILD_BOARD, &(info->Board));
    _get_system_property(ANDROID_OS_BUILD_BRAND, &(info->Brand));
    _get_system_property(ANDROID_OS_BUILD_DEVICE, &(info->Device));
    _get_system_property(ANDROID_OS_BUILD_HARDWARE, &(info->Hardware));
    _get_system_property(ANDROID_OS_BUILD_CPU_ABI, &(info->CPUAbi));
    _get_system_property(ANDROID_OS_BUILD_CPU_ABI2, &(info->CPUAbi2));


    _get_system_property(ANDROID_OS_BUILD_BOOT_MODE, &(info->BootMode));
    _get_system_property(ANDROID_OS_BUILD_BOOT_BASEBAND, &(info->BootBaseBand));
    _get_system_property(ANDROID_OS_BUILD_BOOT_BOOTLOADER, &(info->BootLoader));
    _get_system_property(ANDROID_OS_BUILD_BOOT_HARDWARE, &(info->BootHardware));
    _get_system_property(ANDROID_OS_BUILD_BOOT_SERIALNO, &(info->BootSerialNo));

    _get_system_property(ANDROID_OS_RIL_SERIALNUMBER, &(info->RilSerialNumber));


    _get_system_property(ANDROID_DEVICE_ARCH, &(info->Arch));
    _get_system_property(ANDROID_DEVICE_CHIPNAME, &(info->ChipName));
    _get_system_property(ANDROID_DEVICE_CSC_COUNTRY_CODE, &(info->CSCCountryCode));
    _get_system_property(ANDROID_DEVICE_CSC_COUNTRY_CODE2, &(info->CSCCountryCode2));
    _get_system_property(ANDROID_DEVICE_RIL_PRODUCT_CODE, &(info->ProductCode));
    _get_system_property(ANDROID_DEVICE_GSM_VERSION_BASEBAND, &(info->GSMBaseBand));
    _get_system_property(ANDROID_DEVICE_BASEBAND, &(info->BaseBand));
    _get_system_property(ANDROID_DEVICE_MODEM_BOARD, &(info->ModemBoard));

    _get_system_property(ANDROID_DEVICE_WARRANTY_BIT, &(info->KNOXWarrantyBit));
    _get_system_property(ANDROID_DEVICE_BOOT_WARRANTY_BIT, &(info->KNOXWarrantyBit2));

    return 1;
}


void print_system_properties(android_sys_info* info)
{
    printf("%s:\t%s\n", info->SerialNo.name, info->SerialNo.value);
    printf("%s:\t%s\n", info->Model.name, info->Model.value);
    printf("%s:\t%s\n", info->Imei.name, info->Imei.value);
    printf("%s:\t%s\n", info->Manufacturer.name, info->Manufacturer.value);
    printf("%s:\t%s\n", info->ProductSerialNo.name, info->ProductSerialNo.value);
    printf("%s:\t%s\n", info->Name.name, info->Name.value);
    printf("%s:\t%s\n", info->Platform.name, info->Platform.value);
    printf("%s:\t%s\n", info->Hardware.name, info->Hardware.value);
    printf("%s:\t%s\n", info->Board.name, info->Board.value);
    printf("%s:\t%s\n", info->Brand.name, info->Brand.value);
    printf("%s:\t%s\n", info->Device.name, info->Device.value);
    printf("%s:\t%s\n", info->CPUAbi.name, info->CPUAbi.value);
    printf("%s:\t%s\n", info->CPUAbi2.name, info->CPUAbi2.value);

    printf("%s:\t%s\n", info->BootMode.name, info->BootMode.value);
    printf("%s:\t%s\n", info->BootBaseBand.name, info->BootBaseBand.value);
    printf("%s:\t%s\n", info->BootLoader.name, info->BootLoader.value);
    printf("%s:\t%s\n", info->BootHardware.name, info->BootHardware.value);
    printf("%s:\t%s\n", info->BootSerialNo.name, info->BootSerialNo.value);

    printf("%s:\t%s\n", info->RilSerialNumber.name, info->RilSerialNumber.value);

    printf("%s:\t%s\n", info->Arch.name, info->Arch.value);
    printf("%s:\t%s\n", info->ChipName.name, info->ChipName.value);
    printf("%s:\t%s\n", info->CSCCountryCode.name, info->CSCCountryCode.value);
    printf("%s:\t%s\n", info->CSCCountryCode2.name, info->CSCCountryCode2.value);
    printf("%s:\t%s\n", info->ProductCode.name, info->ProductCode.value);
    printf("%s:\t%s\n", info->GSMBaseBand.name, info->GSMBaseBand.value);
    printf("%s:\t%s\n", info->BaseBand.name, info->BaseBand.value);
    printf("%s:\t%s\n", info->ModemBoard.name, info->ModemBoard.value);
    printf("%s:\t%s\n", info->KNOXWarrantyBit.name, info->KNOXWarrantyBit.value);
    printf("%s:\t%s\n", info->KNOXWarrantyBit2.name, info->KNOXWarrantyBit2.value);

}

