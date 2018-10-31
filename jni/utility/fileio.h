#ifndef __FILE_IO_H__
#define __FILE_IO_H__

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

unsigned int compute_signature(const unsigned char *buffer, size_t size);
unsigned int extract_signature(const char *filename);
void  append_signature(const char *filename, unsigned int signature);


#ifdef __cplusplus
}
#endif


#endif