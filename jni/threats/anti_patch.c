
// build:
// gcc anti_patch_android.c  -D_GNU_SOURCE -ldl


#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <fcntl.h>
#include <dlfcn.h>
#include <unistd.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <sys/mman.h>


#include "utility/fileio.h"
#include "utility/proc_pid_maps.h"
#include "profile/appstatus.h"


// func:   get the mapped code-sections in memory
// input:  library name
// output:  start and end addresses
// return:  length in memory
unsigned int get_loaded_lib_signature(const char* lib_name, size_t *length)
{
	size_t start = 0;
	size_t end = 0;

	size_t len = get_loaded_lib_location(lib_name, &start, &end);
	if ( len == 0 )  return 0;

	// compute the signature for this code section in memory
	unsigned char *mem = (unsigned char *) start;
	unsigned int signature = compute_signature(mem, len);
	*length = len;

	printf("signature for code section: %d\n", signature);
	return  signature;
}




// Another method using mmap to get file signature
// require:  full library path name, and code section length from outside
unsigned int read_library_from_file( const char *lib_name, size_t len)
{
	unsigned int signature = 0;

	// read code-section of lib*.so from physical file on disk
	int fd = open(lib_name, O_RDONLY);
    //struct stat buffer;
    //int status;
    //status = fstat(fd, &buffer);
    //len = buffer.st_size;

	//int fd = open("/lib/x86_64-linux-gnu/libc-2.19.so",  O_RDONLY);
	unsigned char *disk = mmap(0, len, PROT_READ, MAP_PRIVATE, fd, 0);
	{
		signature = compute_signature(disk, len);
	}
	munmap(disk, len);
	close(fd);

	printf("signature mmap: %d\n", signature);

	return signature;
}



// load the library (if necessary) and return signature
unsigned int get_lib_signature(const char *filename,  int require_load)
{
	unsigned int signature = 0;

    void *handle = 0;
	if ( require_load )
	{
		// load the give library, which will then show up in image list
		handle = dlopen(filename, RTLD_LAZY);
		if ( handle == NULL )
		{
			printf("dlopen error: %s\n", dlerror());
			return 0;
		}
	}

	// get signature
	//{
		size_t len;
		signature = get_loaded_lib_signature(filename, &len);
	//}

	if ( require_load )
	{
        // use another method to confirm the obtained signature
        unsigned signature2 = read_library_from_file(filename, len);
        printf("confirmed signature = %d\n", signature2);


		dlclose(handle);   // unload
	}

	return signature;
}




// read signature from file end, and compare against in code memory
// return: 0 -OK, Others-Failed
// usage:  embed this function into your code where needs library check
int validate_signed_library(const char *lib_name)
{
	// note in android, the unpacked library normally will be put into "lib" folder under "/data/data/com.company.application/".
	//const char * lib_name = "/data/data/vkey.android.vtap/lib/libvosWrapperEx.so";

	unsigned int signature_tail = extract_signature(lib_name);

// if embedded code
	size_t len;
	unsigned int signature_mem = get_loaded_lib_signature(lib_name, &len);

	return !(signature_tail == signature_mem);
}


void validate_signed_library2()
{
    int status = validate_signed_library("/data/data/com.sample.app/lib/libsample.so");
    if ( status != 0 )
		{
			//SET_GSTATUS_LIBRARY_TAMPERED();
		}
}


#ifdef  TEST_ANTI_PATCH


int main(int argc, const char *argv[])
{
	if ( argc <  3 )
	{
		printf("Extract/verify library (.so) file signature.\n");
		printf("Usage:\n");
		printf("  %s  -gen libfile.so          : generate signature for, example: libc-2.19.so\n", argv[0]);
		printf("  %s  -append sign libfile.so  : append sign to file\n", argv[0]);
		printf("  %s  -extract libfile.so      : extract sign from file\n", argv[0]);
		printf("  %s  -validate libfile.so     : validate signed so file\n", argv[0]);

		return 0;
	}

	if ( strcmp(argv[1], "-gen") == 0 )
	{
		const char *lib_name = argv[2];  // "/lib/x86_64-linux-gnu/libc-2.19.so"
		//unsigned int signature = read_library_from_file(lib_name, len);

		unsigned int signature = get_lib_signature(lib_name, 1);

		printf("signature file: %d\n", signature);
	}

	else if ( strcmp(argv[1], "-append") == 0 )
	{
		unsigned int signature = atoi(argv[2]);
		const char *lib_name = argv[3];
		append_signature(lib_name, signature);
	}

	else if ( strcmp(argv[1], "-extract") == 0 )
	{
		const char *lib_name = argv[2];
		unsigned int signature = extract_signature(lib_name);
	}

	else if ( strcmp(argv[1], "-validate") == 0 )
	{
		const char *lib_name = argv[2];
		unsigned int signature = extract_signature(lib_name);

		unsigned int signature2 = get_lib_signature(lib_name, 1);

		printf("validate= %s\n", signature == signature2 ? "OK" : "Failed");

	}


	return 0;
}

#endif
