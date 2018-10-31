#ifndef __ANDROID_PROPERTY_GET_H__
#define __ANDROID_PROPERTY_GET_H__

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <dlfcn.h>
#include <sys/system_properties.h>


#include "utility/log.h"

#define printf LOGI


#ifdef __cplusplus
extern "C" {
#endif



//#define  PROP_NAME_MAX  32
//#define  PROP_VALUE_MAX 92




//#define  PROP_NAME_MAX  32
//#define  PROP_VALUE_MAX 92

// 自定义属性单元
typedef struct t_android_sys_info_item
{
    char name[PROP_NAME_MAX];   // 32
    char value[PROP_VALUE_MAX];  // 92. // PROP_VALUE_MAX from <sys/system_properties.h>. defined as 92
    int  len;   // len of value equal to strlen(value), also indicating if it is null
}  android_sys_info_item;









#if (__ANDROID_API__ >= 21)
// Android 'L' makes __system_property_get a non-global symbol.
// Here we provide a stub which loads the symbol from libc via dlsym.
typedef int (*PFN_SYSTEM_PROP_GET)(const char *, char *);
int __system_property_get(const char* name, char* value)
{
    static PFN_SYSTEM_PROP_GET __real_system_property_get = NULL;
    if (!__real_system_property_get)
    {
        // libc.so should already be open, get a handle to it.
        void *handle = dlopen("libc.so", RTLD_NOLOAD);
        if (!handle)   // not yet loaded by anyone
        {
            printf("libc.so not yet loaded -> dlopen RTLD_NOLOAD error: %s.\n", dlerror());
            // let me open it now
            handle = dlopen("libc.so", RTLD_LAZY);   // | RTLD_LOCAL ??
            if (!handle)
            {
               printf("myself cannot dlopen libc.so either: %s.\n", dlerror());
               return 0;
            }
        }

        __real_system_property_get = (PFN_SYSTEM_PROP_GET)dlsym(handle, "__system_property_get");
        if (!__real_system_property_get)
        {
            printf("Cannot resolve __system_property_get(): %s.\n", dlerror());
            return 0;
        }

        printf("resolve __system_property_get() successfully.\n");
    }

    return (*__real_system_property_get)(name, value);
}
#endif // __ANDROID_API__ >= 21





int property_get(const char *key, char *value, const char *default_value);


void _get_system_property(const char* name, android_sys_info_item* item);





#ifdef __cplusplus
}
#endif

#endif /* __ANDROID_PROPERTY_GET_H__ */