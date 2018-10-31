#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#include <dlfcn.h>
#include <fcntl.h>
#include <sys/mman.h>

#include "utility/linker.h"    // define soinfo structure
#include "utility/proc_pid_maps.h"
#include "utility/log.h"
#include "profile/appstatus.h"



// GOT Hook check:
// 检查当前库的符号表中指定函数名(funcname)的地址是否在指定库(libname)的地址范围内.

// 也参考 LD_PRELOAD detection.


static int get_current_so_path(char* outPath, int len)
{
	char path[151] ={0};
	char maps_path[30];
	char line[250];
	long s;

	int found = 0;

	// obtain the file path of current .so file
	sprintf(maps_path, "/proc/%d/maps", (int) getpid());
	FILE *f = fopen(maps_path, "r");
	if (!f) return 0;

	while (fgets(line, sizeof line, f) != NULL)
	{
		if (strlen(line) <= 50) {
			continue;
		}
		sscanf(line, "%lx-%*x %*s %*x %*x:%*x %*d %150s", &s, path);
		//if ((long)validate_GOT_hook < s) 			// 找到当前 library, 这种方式可能需要改进
		if (strstr(path, "libmukey.so") != 0)   // TODO:
		{
			LOGI("found current .so path : %s\n", path);
			found = 1;
			break;
		}
	}
	fclose(f);

  // copy result
	memset(outPath, 0, len);
	if (found && strlen(path) + 1 <=len)
	{
     strncpy(outPath, path, strlen(path));
		 found = strlen(path);
		 return found;
  }

	LOGE("current .so path not found or length error!\n");
	return 0;
}


int validate_GOT_hook(const char *funcname, const char* libname)
{
	int i;
	int isTampered = 0;

	// 查找 /proc/pid/maps，以获得地址范围
	long start = 0, end = 0;
  size_t len = get_loaded_lib_location(libname, &start, &end);
	 if (!len)
	    return 0;

	// obtain the file path of current .so file
	char path[151];
  int result = get_current_so_path(path, sizeof(path));
	if (!result)
	    return 0;

  // 解析该library file.
	soinfo* si = (soinfo*)dlopen(path, RTLD_NOW);
	if (si == NULL || si->strtab == NULL || si->plt_rel == NULL)
	{
		// 未找到符号表
		return 0;
	}

	// parsing ELF header and obtain function address in GOT
	// check if the function address belongs to specific library (e.g. libc.so)
	for (i = 0; i < si->plt_rel_count; i++)
	{
		if (strcmp(si->symtab[ELF32_R_SYM(si->plt_rel[i].r_info)].st_name + si->strtab, funcname) == 0)
		{
			uint32_t* got = (uint32_t*)(si->base + si->plt_rel[i].r_offset);
			if (!((*got > start) && (*got < end)))
			{
				LOGE("GOT hook: function %s address was tampered in %s GOT: %x\n", funcname, path, *got);
				isTampered = 1;
				set_got_hook_status();
				break;
			}
		}
	}

	dlclose(si);
	return isTampered;
}




// Inline hook check:
// 检查载入的库内存是否与硬盘上文件内容一致，如不一致则认为有inline hook发生。
// 此时并输出被hooked function.

int validate_inline_hook(const char* libname)
{
	// 查找 /proc/pid/maps，以获得地址范围
	long start = 0, end = 0;
  size_t len = get_loaded_lib_location(libname, &start, &end);
	if (!len)
	{
	    return 0;
	}

	int isTampered = 0;
	int i = 0;
	unsigned char *mem = (unsigned char *) start;

	// read code-section of libc.so from physical file on disk
	int fd = open(libname, O_RDONLY);
	if (fd == -1)
	{
		LOGE("warning: unable to open file %s\n", libname);
		return 0;
	}

	// do memory map
	unsigned char *disk = mmap(0, end - start, PROT_READ, MAP_PRIVATE, fd, 0);
	if (disk == -1)
	{
		LOGE("warning: mmap error %s\n", strerror(errno));
		close(fd);
		return 0;
	}

	// compare the mapped code-sections on memory with code-section which obtained from physical file
	for (i = 0; i < end - start; i++)
	{
		if ((mem[i] != disk[i]))  // memory内容不一致
		{
			LOGE("inline hook: memory mismatch of lib %s\n", libname);
			// 谨慎起见，解析出函数地址，多做检查
			Dl_info tmp;
			dladdr((const void *) (start + i), &tmp);
			if (tmp.dli_sname == NULL)
				continue;    // 非有效函数名
			if (tmp.dli_saddr == 0)
				continue;    // 非有效函数地址

      // 是否需要过滤函数: 只检查指定函数是否被hooked
			/*
      if (strcmp(tmp.dli_sname, funcname) != 0)
			  continue;
				*/

			// 找到函数
			LOGE("hooked inline function: sname=%s, saddr=%p\n", tmp.dli_sname, tmp.dli_saddr);
			isTampered = 1;
			set_inline_hook_status();

			break;
		}
	}

	munmap(disk, end - start);
	close(fd);

	return isTampered;
}





//define C functions for GOT verification

// libc.so其下有这些函数
#define system_lib_libc_cstr                 "/system/lib/libc.so"
#define malloc_cstr                          "malloc"
#define memcpy_cstr                          "memcpy"
#define strcmp_cstr                          "strcmp"
#define strstr_cstr                          "strstr"
#define sscanf_cstr                          "sscanf"
#define fgets_cstr                           "fgets"
#define fopen_cstr                           "fopen"
#define time_cstr                            "time"
#define __system_property_get_cstr           "__system_property_get"


// 检查时间函数
#define libutils_cstr                        "/system/lib/libutils.so"
#define elapsedRealtime_cstr                 "_ZN7android15elapsedRealtimeEv"


int check_got_hook()
{
	int isHooked = 0;

  LOGI("GOT hook check: started.\n");

	isHooked = isHooked || validate_GOT_hook(malloc_cstr, system_lib_libc_cstr);
	isHooked = isHooked || validate_GOT_hook(memcpy_cstr, system_lib_libc_cstr);
	isHooked = isHooked || validate_GOT_hook(strcmp_cstr, system_lib_libc_cstr);
	isHooked = isHooked || validate_GOT_hook(strstr_cstr, system_lib_libc_cstr);
	isHooked = isHooked || validate_GOT_hook(sscanf_cstr, system_lib_libc_cstr);
	isHooked = isHooked || validate_GOT_hook(fgets_cstr,  system_lib_libc_cstr);
	isHooked = isHooked || validate_GOT_hook(fopen_cstr,  system_lib_libc_cstr);
	isHooked = isHooked || validate_GOT_hook(time_cstr,   system_lib_libc_cstr);
	isHooked = isHooked || validate_GOT_hook(__system_property_get_cstr, system_lib_libc_cstr);

	isHooked = isHooked || validate_GOT_hook(elapsedRealtime_cstr, libutils_cstr);

  LOGI("GOT hook check: end.\n");

	return isHooked;
}







// API
int check_inline_hook()
{
	int  isHooked = 0;

  LOGI("inline hook check: started.\n");

  // 首先检查一些系统库hook的情况
	isHooked = isHooked || validate_inline_hook(system_lib_libc_cstr);
	isHooked = isHooked || validate_inline_hook(libutils_cstr);

  // 然后检查自身library hook的情况
	// bug: 检查自身的时候mmap会出错!
	// isHooked = isHooked || validate_inline_hook("/data/app-lib/com.nlog2n.mukey-2/libmukey.so");
  /*
	// obtain the file FULL path of current .so file
	char path[151];
  int result = get_current_so_path(path, sizeof(path));
	if (result)
	{
		isHooked = isHooked || validate_inline_hook(path); // "libmukey.so");
  }
	*/

	LOGI("inline hook check: end.\n");

	return isHooked;
}
