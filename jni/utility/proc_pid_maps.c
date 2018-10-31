#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <fcntl.h>
#include <dlfcn.h>
#include <unistd.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <sys/mman.h>

#include "utility/log.h"
#include "utility/proc_pid_maps.h"


// build:
// gcc proc_pid_maps.c  -D_GNU_SOURCE -ldl

// 关于loaded native shared library的操作，包括:
//   1. 列出所有载入库文件名，基地址, 偏移.
//   2. 检查某函数指针是否在指定库地址范围内.

// 可继续参考我在memdump/下实现的代码.



// get file name without path
const char * strip_filename(const char* lib_name)
{
    int i;

    if ( !lib_name ) return lib_name;

    for (i = strlen(lib_name) - 1; i >= 0; i-- )
    {
        if ( *(lib_name + i) == '/' )
        {
            return (lib_name + i + 1);
        }
    }

    return lib_name;
}


// from anti_patch.c

// also refer to anti_patch.c  get_loaded_lib_signature() function
size_t get_loaded_lib_location(const char* lib_name, size_t *start, size_t *end)
{
    *start = 0;
    *end = 0;

    char maps_path[30] = {0};
    sprintf(maps_path, proc_pid_maps_cstr, (int)getpid());
    FILE *f = fopen(maps_path, "r");
    if (f == NULL)
    {
        LOGE("fopen error: %s", maps_path);
        return 0;
    }

    // keep lib filename only, not necessary if the path is precise.
    lib_name = strip_filename(lib_name);

    char line[250] = {0};
    while (fgets(line, sizeof line, f) != NULL)
    {
        if (strlen(line) <= 50)
        {
            continue;
        }

        // get one line. note that myself app will also show up in the first (serveral) rows

        //printf("line = %s\n", line);
        char path[151];
        char permission[5];
        size_t s, e, offset;    // should i use %lx to read ?
        // here offset is for offset in file
        sscanf(line, "%x-%x %4s %x %*x:%*x %*d %150s",&s,&e,permission,&offset,path);
        // sample:
        // start_address end_address  permission .... path
        // 7f7f6befa000-7f7f6bf01000 r-xp 00000000 08:01 2363702                    /lib/x86_64-linux-gnu/librt-2.19.so

        if (strstr(path, lib_name) != 0)
        {
            // any particular interest on lib as framework_java_classes_dex_cstr?

            printf("path=%s [%lx-%lx] %s\n", path,
              (long unsigned int)s,
              (long unsigned int)e,
              permission);

            // look for executable like "r-xp".
            if (strstr(permission, "x") != NULL)
            {
                *start = s;
                *end = e;
                break;
            }
        }

    }
    fclose(f);

    printf( "start = 0x%lx, end = 0x%lx\n",
       (long unsigned int)*start,
       (long unsigned int)*end);
    size_t len = *end - *start;
    return len;
}



// verify whether a function pointer is indeed inside the address range of a known loaded library.
int is_funcaddr_valid(void *func, const char* library_name)
{
    size_t start, end;
    if (!get_loaded_lib_location(library_name, &start, &end))
    {
      return 0; // error
    }

    if ((size_t) func >= start && (size_t) func <= end)
        return 1;

    LOGE("func addr %p is NOT in [%0x - %0x]", func, start, end);
    return 0;
}
