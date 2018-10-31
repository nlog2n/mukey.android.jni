#ifndef  __LD_PRELOAD_H__
#define  __LD_PRELOAD_H__


#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
//#include <sys/system_properties.h>

#include <fcntl.h>
#include <dlfcn.h>
#include <unistd.h>


#define libc_env_variable_mac                "DYLD_INSERT_LIBRARIES"
#define libc_env_variable_linux              "LD_PRELOAD"
#define libc_env_variable   libc_env_variable_linux

// specify trusted library path here to resist preloaded library injection
#define libc_cstr_android                    "/system/lib/libc.so"
#define libc_cstr_linux                      "/lib/x86_64-linux-gnu/libc.so.6"
#define libc_cstr_mac                        "/usr/lib/libc.dylib"
#define libc_cstr    libc_cstr_android

#define time_cstr                            "time"
#define strcmp_cstr                          "strcmp"
#define strstr_cstr                          "strstr"
#define memcpy_cstr                          "memcpy"
#define fgets_cstr                           "fgets"
#define fopen_cstr                           "fopen"
#define sscanf_cstr                          "sscanf"
#define malloc_cstr                          "malloc"
#define free_cstr                            "free"
//#define rand_cstr                            "rand"
// android notes: with NDK 64 bit, some stdlib functions cannot
//  be found any more (like rand/srand/atof).

//#define __system_property_get_cstr           "__system_property_get"
// android notes:   __system_property_get was removed from Android L.


// syscalls
typedef struct _libc_function_pointers
{
	void*    (*mallocptr) (size_t);
	void     (*freeptr)   (void*);
	time_t   (*timeptr)   (time_t *t);
	int      (*strcmpptr) (char *string1, char *string2);
	char*    (*strstrptr) (const char *haystack, const char *needle);
	void*    (*memcpyptr) (void *dest, const void *src, size_t count);
	char*    (*fgetsptr)  (char *str, int n, FILE *stream);
	FILE*    (*fopenptr)  (const char *filename, const char *mode);
	int      (*sscanfptr) (const char *str, const char *format, ...);
	//int      (*randptr)   (void);

}  t_libc_function_pointers;







#ifdef __cplusplus
extern "C" {
#endif


// init or verify LIBC function pointers
// verify single function pointer
// return: 0-OK, 1-fail/hooked
int init_libc_function_pointers(t_libc_function_pointers*  g_func_pointers);


#ifdef __cplusplus
}
#endif


#endif

