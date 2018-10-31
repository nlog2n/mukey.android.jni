#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif

#include <stdio.h>
#include <stdlib.h>

#include <dlfcn.h>
#include <fcntl.h>
#include <unistd.h>

#include "utility/log.h"
#include "utility/proc_pid_maps.h"

// build:
// gcc list_loaded_library_linux.c -ldl


// list all the loaded shared library in runtime
// by command:
// use the /proc/self/maps path where self is a symlink to the current process
// #cat /proc/pid/maps

// lists all sorts of useful information, for example, if the process is java, lists all the open jars.
// #lsof -p pid

// strace is best when child process might be spawned
// #strace -f



// check /proc/pid/maps to see any suspicious libraries injected by app.
// refer to:  proc_pid_maps.c/get_loaded_lib_location()
int validate_loaded_library()
{
	int found = 0;
  const char* suspicious_lib_name = "frida";
	const char* suspicious_lib2_name = "cycript";

	char maps_path[30] = {0};
	sprintf(maps_path, proc_pid_maps_cstr, (int)getpid());
	FILE *f = fopen(maps_path, "r");
	if (f == NULL)
	{
			LOGE("fopen error: %s", maps_path);
			return 0;  // assumed OK
	}

	char line[250] = {0};
	while (fgets(line, sizeof line, f) != NULL)
	{
			char path[151];
			char permission[5];
			size_t s, e, file_offset;
			sscanf(line, "%x-%x %4s %x %*x:%*x %*d %150s",&s,&e,permission,&file_offset,path);

					// only look for executable like "r-xp".
				if (strstr(permission, "x") != NULL)
			{
				 // LOGI("line = %s\n", line);

				 // found suspicious lib name
				 if (strstr(path, suspicious_lib_name) != 0)
					{
						found = 1;
						LOGE("found suspicious library: %s", suspicious_lib_name);
						//break;
					}

					if (strstr(path, suspicious_lib2_name) != 0)
 					{
 						found = 2;
 						LOGE("found suspicious library: %s", suspicious_lib2_name);
 						//break;
 					}

			}

	}

	fclose(f);
  return 0;
}





#ifdef _TEST_LINUX_LOADED_LIBRARY_

// 使用 link_map获取载入库名单的方法只适用于 Linux，但Android上不可以.
// 另 Mac and iOS 有自己的dylib get image list方法.
//
// refer to: Listing Loaded Shared Objects in Linux
// http://syprog.blogspot.sg/2011/12/listing-loaded-shared-objects-in-linux.html
// http://stackoverflow.com/questions/17620751/use-dlinfo-to-print-all-symbols-in-a-library



// #include <link.h>
// for struct link_map
// Android 下面找不到完整定义，报错:
//   ndk error: dereferencing pointer to incomplete type
// 可自定义，见reference link
struct lmap
{
	void*    base_address;   // Base address of the shared object
	char*    path;           // Absolute file name (path) of the shared object
	void*    not_needed1;    // Pointer to the dynamic section of the shared object
	struct lmap *next, *prev;// chain of loaded objects
};

struct something {
	void*  pointers[3];
	struct something* ptr;
};


int list_loaded_library()
{
	// get handle for main
	void* handle = dlopen(NULL, RTLD_NOW);
	if (!handle)
	{
		printf("dlopen error: %s\n", dlerror());
		return 0;
	}

	struct something *p = (struct something*) handle;
  p = p->ptr;   // second
	if (!p)
	{
		printf("parse dlopen handle error.\n");
		dlclose(handle);
		return 0;
	}
	p = p->ptr;   // third
	if (!p)
	{
		printf("parse dlopen handle error 2.\n");
		dlclose(handle);
		return 0;
	}

    // finally gets us to the first link_map structure
	struct lmap *map = (struct lmap*) p;
	while (map)
	{
		printf("check loaded lib: %s\n", (const char*)map->path);

		// you can do something with |map| like with handle returned by |dlopen()|.

		map = map->next;
	}

	dlclose(handle);
	return 0;
}


int list_loaded_library_2(const char* libpath)
{
  void*  handle = dlopen(libpath, RTLD_NOW);
	if (!handle)
	{
		printf("dlopen error: %s\n", dlerror());
		return 0;
	}

  // Get the link map
   struct lmap* link_map = 0;
   int ret = dlinfo(handle, RTLD_DI_LINKMAP, &link_map); // RTLD_DI_LINKMAP =2
   if (!ret || !link_map)
	 {
		 printf("error.\n");
		 return 0;
	 }

  printf("Libraries:\n");
  while (link_map->prev)
	{
    link_map = link_map->prev;
  }

  while (link_map->next)
	{
    printf(" - %s (0x%X)\n", link_map->path, link_map->base_address);
    link_map = link_map->next;
  }
  dlclose(handle);
  return 0;
}

int main()
{
	list_loaded_library();
	validate_loaded_library();
	return 0;
}
#endif
