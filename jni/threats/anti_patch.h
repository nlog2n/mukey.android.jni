#ifndef __ANTI_PATCH_H__
#define __ANTI_PATCH_H__

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <fcntl.h>
#include <dlfcn.h>
#include <unistd.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <sys/mman.h>


#ifdef __cplusplus
extern "C" {
#endif


// get file name without path
const char * strip_filename(const char* lib_name);

size_t get_loaded_lib_location(const char* lib_name, size_t *start, size_t *end);

// func:   get the mapped code-sections in memory
// input:  library name
// output:  start and end addresses
// return:  length in memory
unsigned int get_loaded_lib_signature(const char* lib_name, size_t *length);


// Another method using mmap to get file signature
// require:  full library path name, and code section length from outside
unsigned int read_library_from_file( const char *lib_name, size_t len);


// load the library (if necessary) and return signature
unsigned int get_lib_signature(const char *filename,  int require_load);


// read signature from file end, and compare against in code memory
// return: 0 -OK, Others-Failed
// usage:  embed this function into your code where needs library check
int validate_signed_library(const char *lib_name);


void validate_signed_library2();



#ifdef __cplusplus
}
#endif


#endif