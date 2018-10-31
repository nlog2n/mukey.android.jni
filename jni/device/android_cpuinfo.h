#ifndef __ANDROID_CPUINFO_H__
#define __ANDROID_CPUINFO_H__


#include <stdlib.h>
#include <stdio.h>
#include <string.h>

// generic info
#define CPU_INFO_PROCESSOR         "processor"
#define CPU_INFO_VENDOR_ID         "vendor_id"
#define CPU_INFO_CPU_FAMILY        "cpu family"
#define CPU_INFO_MODEL             "model"
#define CPU_INFO_MODEL_NAME        "model name"
#define CPU_INFO_STEPPING          "stepping"
#define CPU_INFO_MICROCODE         "microcode"
#define CPU_INFO_CPU_MHZ           "cpu MHz"
#define CPU_INFO_CACHE_SIZE        "cache size"
#define CPU_INFO_PHYSICAL_ID       "physical id"
#define CPU_INFO_SIBLINGS          "siblings"
#define CPU_INFO_CORE_ID           "core id"
#define CPU_INFO_CPU_CORES         "cpu cores"
#define CPU_INFO_APICID            "apicid"
#define CPU_INFO_INITIAL_APICID    "initial apicid"
#define CPU_INFO_FDIV_BUG          "fdiv_bug"
#define CPU_INFO_F00F_BUG          "f00f_bug"
#define CPU_INFO_COMA_BUG          "coma_bug"
#define CPU_INFO_FPU               "fpu"
#define CPU_INFO_FPU_EXCEPTION     "fpu_exception"
#define CPU_INFO_CPU_LEVEL         "cpuid level"
#define CPU_INFO_WP                "wp"
#define CPU_INFO_FLAGS             "flags"
#define CPU_INFO_BOGOMIPS          "bogomips"
#define CPU_INFO_CLFLUSH_SIZE      "clflush size"
#define CPU_INFO_CACHE_ALIGNMENT   "cache_alignment"
#define CPU_INFO_ADDRESS_SIZES     "address sizes"
#define CPU_INFO_POWER_MANAGEMENT  "power management"


// these attributes shown up in Samsung Galaxy S6
#define CPU_INFO_S_PROCESSOR       "Processor"
#define CPU_INFO_FEATURES          "Features"
#define CPU_INFO_CPU_IMPLEMENTER   "CPU implementer"
#define CPU_INFO_CPU_ARCHITECTURE  "CPU architecture"
#define CPU_INFO_CPU_VARIANT       "CPU variant"
#define CPU_INFO_CPU_PART          "CPU part"
#define CPU_INFO_CPU_REVISION      "CPU revision"
#define CPU_INFO_HARDWARE          "Hardware"



// attributes for Meizu
#define CPU_INFO_Cluster           "Cluster"           // : CA15
#define CPU_INFO_BOGOMIPS_2        "BogoMIPS"          // : 1590.88   // 与上面bogomips同
#define CPU_INFO_CPU_ASV_GROUP     "CPU asv group"     // : 9
#define CPU_INFO_CPU_ASV_VERSION   "CPU asv version"   // : 1
#define CPU_INFO_CPU_BOOT_CLUSTER  "CPU boot cluster"  // : CA15
#define CPU_INFO_REVISION          "Revision"          // : 5410
#define CPU_INFO_DMEM              "DMEM"              // : D25
#define CPU_INFO_SERIAL            "Serial"            // : 0000000000000000


typedef struct t_cpu_info
{
    // CPU 个数
    int num_processors;

    // 只记录最后一个CPU信息
    char   vendor_id[64];
    int    cpu_family;
    int    model;
    char   model_name[128];
    int    stepping;
    int    microcode;
    char   cpu_MHz[16];
    char   cache_size[16];
    int    physical_id;
    int    siblings;
    int    core_id;
    int    cpu_cores;
    int    apicid;
    int    initial_apicid;
    char   fdiv_bug[4];
    char   f00f_bug[4];
    char   coma_bug[4];
    char   fpu[4];
    char   fpu_exception[4];
    int    cpuid_level;
    char   wp[4];
    char   flags[256];
    char   bogomips[16];
    int    clflush_size;
    int    cache_alignment;
    char   address_sizes[64];
    char   power_management[64];

    // for samsung android phones
    char   samsung_processor[64];
    char   samsung_features[128];    // 运行时值和静态查看时不一样，弃用
    int    samsung_cpu_implementer;
    char   samsung_cpu_architecture[16];  // 运行时值和静态查看时不一样，弃用
    int    samsung_cpu_variant;
    int    samsung_cpu_part;
    int    samsung_cpu_revision;
    char   samsung_hardware[64];

    // meizu
    char   meizu_cluster[16];
    //char   meizu_bogomips[16];
    int    meizu_cpu_asv_group;
    int    meizu_cpu_asv_version;
    char   meizu_cpu_boot_cluster[16];
    int    meizu_revision;
    char   meizu_dmem[16];
    char   meizu_serial[32];

}  cpu_info;



#ifdef __cplusplus
extern "C" {
#endif

void print_android_cpuinfo(cpu_info* info);

// read device info from /proc/cpuinfo
// return:  0 fail, 1 success
int get_android_cpuinfo(cpu_info* info);


#ifdef __cplusplus
}
#endif


#endif