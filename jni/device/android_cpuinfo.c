#include "android_cpuinfo.h"

#include "utility/log.h"
#define printf LOGI


// read device info from /proc/cpuinfo
// return:  0 fail, 1 success
int get_android_cpuinfo(cpu_info* info)
{
    memset(info, 0, sizeof(cpu_info));

    FILE *f = fopen("/proc/cpuinfo", "r");
    if (f == NULL)    return 0;

    char line[256] = {0};
    while (fgets(line, sizeof line, f) != NULL)
    {
        char key[128] = {0};
        char val[128] = {0};
        sscanf(line, "%[^\t:] : %[^\t\n]", key, val);
        if ( !key || key[0] == '\n' || key[0] == '\r' )
            continue;

        //printf("%s", line);
        //printf("%s=%s\n", key, val);

        if (strcmp(key, CPU_INFO_PROCESSOR) == 0)
        {
            info->num_processors ++;
        }

        if (strcmp(key, CPU_INFO_VENDOR_ID) == 0)
        {
            strncpy(info->vendor_id, val, sizeof(info->vendor_id)-1);
        }

        if (strcmp(key, CPU_INFO_CPU_FAMILY) == 0)
        {
            info->cpu_family = atoi(val);
        }

        if (strcmp(key, CPU_INFO_MODEL) == 0)
        {
            info->model = atoi(val);
        }

        if (strcmp(key, CPU_INFO_MODEL_NAME) == 0)
        {
            strncpy(info->model_name, val, sizeof(info->model_name)-1);
        }

        if (strcmp(key, CPU_INFO_STEPPING) == 0)
        {
            info->stepping = atoi(val);
        }

        if (strcmp(key, CPU_INFO_MICROCODE) == 0)
        {
            info->microcode = strtol(val, 0, 16);
        }

        if (strcmp(key, CPU_INFO_CPU_MHZ) == 0)
        {
            strncpy(info->cpu_MHz, val, sizeof(info->cpu_MHz)-1);
        }

        if (strcmp(key, CPU_INFO_CACHE_SIZE) == 0)
        {
            strncpy(info->cache_size, val, sizeof(info->cache_size)-1);
        }

        if (strcmp(key, CPU_INFO_PHYSICAL_ID) == 0)
        {
            info->physical_id = atoi(val);
        }

        if (strcmp(key, CPU_INFO_SIBLINGS) == 0)
        {
            info->siblings = atoi(val);
        }

        if (strcmp(key, CPU_INFO_CORE_ID) == 0)
        {
            info->core_id = atoi(val);
        }

        if (strcmp(key, CPU_INFO_CPU_CORES) == 0)
        {
            info->cpu_cores = atoi(val);
        }

        if (strcmp(key, CPU_INFO_APICID) == 0)
        {
            info->apicid = atoi(val);
        }

        if (strcmp(key, CPU_INFO_INITIAL_APICID) == 0)
        {
            info->initial_apicid = atoi(val);
        }

        if (strcmp(key, CPU_INFO_FDIV_BUG) == 0)
        {
            strncpy(info->fdiv_bug, val, sizeof(info->fdiv_bug)-1);
        }

        if (strcmp(key, CPU_INFO_F00F_BUG) == 0)
        {
            strncpy(info->f00f_bug, val, sizeof(info->f00f_bug)-1);
        }

        if (strcmp(key, CPU_INFO_COMA_BUG) == 0)
        {
            strncpy(info->coma_bug, val, sizeof(info->coma_bug)-1);
        }

        if (strcmp(key, CPU_INFO_FPU) == 0)
        {
            strncpy(info->fpu, val, sizeof(info->fpu)-1);
        }

        if (strcmp(key, CPU_INFO_FPU_EXCEPTION) == 0)
        {
            strncpy(info->fpu_exception, val, sizeof(info->fpu_exception)-1);
        }

        if (strcmp(key, CPU_INFO_CPU_LEVEL) == 0)
        {
            info->cpuid_level = atoi(val);
        }

        if (strcmp(key, CPU_INFO_WP) == 0)
        {
            strncpy(info->wp, val, sizeof(info->wp)-1);
        }

        if (strcmp(key, CPU_INFO_FLAGS) == 0)
        {
            strncpy(info->flags, val, sizeof(info->flags)-1);
        }

        // updates for meizu
        if (strcmp(key, CPU_INFO_BOGOMIPS) == 0  || strcmp(key, CPU_INFO_BOGOMIPS_2) == 0)
        {
            strncpy(info->bogomips, val, sizeof(info->bogomips)-1);
        }

        if (strcmp(key, CPU_INFO_CLFLUSH_SIZE) == 0)
        {
            info->clflush_size = atoi(val);
        }

        if (strcmp(key, CPU_INFO_CACHE_ALIGNMENT) == 0)
        {
            info->cache_alignment = atoi(val);
        }

        if (strcmp(key, CPU_INFO_ADDRESS_SIZES) == 0)
        {
            strncpy(info->address_sizes, val, sizeof(info->address_sizes)-1);
        }

        if (strcmp(key, CPU_INFO_POWER_MANAGEMENT) == 0)
        {
            strncpy(info->power_management, val, sizeof(info->power_management)-1);
        }


        // for samsung phones
        if (strcmp(key, CPU_INFO_S_PROCESSOR) == 0)
        {
            strncpy(info->samsung_processor, val, sizeof(info->samsung_processor)-1);
        }



        if (strcmp(key, CPU_INFO_FEATURES) == 0)
        {
            printf("%s--%s\n", key, val);
            strncpy(info->samsung_features, val, sizeof(info->samsung_features)-1);
        }



        if (strcmp(key, CPU_INFO_CPU_IMPLEMENTER) == 0)
        {
            info->samsung_cpu_implementer = strtol(val, 0, 16);
        }



        if (strcmp(key, CPU_INFO_CPU_ARCHITECTURE) == 0)
        {
             printf("%s--%s\n", key, val);
            strncpy(info->samsung_cpu_architecture, val, sizeof(info->samsung_cpu_architecture)-1);
        }



        if (strcmp(key, CPU_INFO_CPU_VARIANT) == 0)
        {
            info->samsung_cpu_variant = strtol(val, 0, 16);
        }


        if (strcmp(key, CPU_INFO_CPU_PART) == 0)
        {
            info->samsung_cpu_part = strtol(val, 0, 16);
        }


        if (strcmp(key, CPU_INFO_CPU_REVISION) == 0)
        {
            info->samsung_cpu_revision = atoi(val);
        }


        if (strcmp(key, CPU_INFO_HARDWARE) == 0)
        {
            strncpy(info->samsung_hardware, val, sizeof(info->samsung_hardware)-1);
        }


        // meizu
        if (strcmp(key, CPU_INFO_Cluster) == 0)
        {
            strncpy(info->meizu_cluster, val, sizeof(info->meizu_cluster) - 1);
        }

        /*
        if (strcmp(key, CPU_INFO_BOGOMIPS_2) == 0)
        {
            strncpy(info->meizu_bogomips, val, sizeof(info->meizu_bogomips) - 1);
        }
        */

        if (strcmp(key, CPU_INFO_CPU_ASV_GROUP) == 0)
        {
            info->meizu_cpu_asv_group = atoi(val);
        }

        if (strcmp(key, CPU_INFO_CPU_ASV_VERSION) == 0)
        {
            info->meizu_cpu_asv_version = atoi(val);
        }

        if (strcmp(key, CPU_INFO_CPU_BOOT_CLUSTER) == 0)
        {
            strncpy(info->meizu_cpu_boot_cluster, val, sizeof(info->meizu_cpu_boot_cluster) - 1);
        }

        if (strcmp(key, CPU_INFO_REVISION) == 0)
        {
            info->meizu_revision = atoi(val);
        }

        if (strcmp(key, CPU_INFO_DMEM) == 0)
        {
            strncpy(info->meizu_dmem, val, sizeof(info->meizu_dmem) - 1);
        }

        if (strcmp(key, CPU_INFO_SERIAL) == 0)
        {
            strncpy(info->meizu_serial, val, sizeof(info->meizu_serial) - 1);
        }

        // meuzu end


    }

    fclose(f);
    return 1;
}



void print_android_cpuinfo(cpu_info* info)
{
    printf("cpu info:\n");
    printf("num_processors :%d\n", info->num_processors);
    printf("vendor_id      :%s\n", info->vendor_id);
    printf("cpu_family     :%d\n", info->cpu_family);
    printf("model          :%d\n", info->model);
    printf("model_name     :%s\n", info->model_name);
    printf("stepping       :%d\n", info->stepping);
    printf("microcode      :0x%x\n", info->microcode);
    printf("cpu_MHz        :%s\n", info->cpu_MHz);
    printf("cache_size     :%s\n", info->cache_size);
    printf("physical_id    :%d\n", info->physical_id);
    printf("siblings       :%d\n", info->siblings);
    printf("core_id        :%d\n", info->core_id);
    printf("cpu_cores      :%d\n", info->cpu_cores);
    printf("apicid         :%d\n", info->apicid);
    printf("initial_apicid :%d\n", info->initial_apicid);
    printf("fdiv_bug       :%s\n", info->fdiv_bug);
    printf("f00f_bug       :%s\n", info->f00f_bug);
    printf("coma_bug       :%s\n", info->coma_bug);
    printf("fpu            :%s\n", info->fpu);
    printf("fpu_exception  :%s\n", info->fpu_exception);
    printf("cpuid_level    :%d\n", info->cpuid_level);
    printf("wp             :%s\n", info->wp);
    printf("flags          :%s\n", info->flags);
    printf("bogomips       :%s\n", info->bogomips);
    printf("clflush_size   :%d\n", info->clflush_size);
    printf("cache_alignment:%d\n", info->cache_alignment);
    printf("address_sizes  :%s\n", info->address_sizes);
    printf("power_management:%s\n", info->power_management);


    printf("Processor      :%s\n", info->samsung_processor);
    printf("Features       :%s\n", info->samsung_features);
    printf("CPU implementer:0x%x\n", info->samsung_cpu_implementer);
    printf("CPU architecture:%s\n", info->samsung_cpu_architecture);
    printf("CPU variant    :0x%x\n", info->samsung_cpu_variant);
    printf("CPU part       :0x%x\n", info->samsung_cpu_part);
    printf("CPU revision   :%d\n", info->samsung_cpu_revision);
    printf("Hardware       :%s\n", info->samsung_hardware);

    // meizu
    printf("Cluster        :%s\n", info->meizu_cluster);
    //printf("BogoMIPS       :%s\n", info->meizu_bogomips);
    printf("CPU asv group  :%d\n", info->meizu_cpu_asv_group);
    printf("CPU asv version:%d\n", info->meizu_cpu_asv_version);
    printf("CPU boot cluster:%s\n", info->meizu_cpu_boot_cluster);
    printf("Revision       :%d\n", info->meizu_revision);
    printf("DMEM           :%s\n", info->meizu_dmem);
    printf("Serial         :%s\n", info->meizu_serial);
}


