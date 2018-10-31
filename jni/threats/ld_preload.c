// build:
// linux: gcc ld_preload.c -D_GNU_SOURCE -ldl
// the output:
//    without preloaded library,
//                 dlsym load  = NEXT = Default  (most cases)
//                 dlsym load != Next, however dlsym load = Default (due to diff libc versions only)
//    with    preloaded library,
//                 dlsym load (libc) != ( NEXT = Default) (preloaded lib)

//  mac:  gcc ld_preload.c
// -Wl,-sectcreate,__RESTRICT,__restrict,/dev/null
// this restriction options disable dyld load.
// see http://www.samdmarshall.com/blog/blocking_code_injection_on_ios_and_os_x.html
// if no restriction built, the output:
//    without preloaded library,   dlsym load = NEXT = Default
//    with    preloaded library,   dlsym load = NEXT != Default (preloaded lib)
//                                 however, dlsym path shown as /usr/lib/system/libsystem_c.dylib


// logic:
//     dlsym should be always correct one (from libc).
//     if preloaded, the default value will show difference.
//     next only varies for diff libc versions (same as dlsym most time). no use for this detection purpose.
/*   if no error, return default
     if  (dlsym != next):
         check lib path further
     else:
         if ( dlsym != default)
             check lib path further
*/

#include "utility/log.h"
#include "ld_preload.h"


// routine:  check if any two addresses found for same function symbol
// input:    handle - dlopen handler, func_cstr - function string name
// return:  valid func pointer if unhooked,  0 otherwise.
static void* PRELOAD_CHECK( void* handle, const char* func_cstr )
{
	void *(*libc_func)();
	void *(*ref_func)();   // inferred from RTLD_DEFAULT

	libc_func = dlsym(handle, func_cstr);
	ref_func = dlsym(RTLD_DEFAULT, func_cstr);

	if ( libc_func == NULL )
	{
	    LOGE("dlsym did not find %s\n", func_cstr);
	    return 0;  // error
	}
	if ( ref_func  == NULL )  return libc_func;

	if (  libc_func != ref_func )
	{
	    LOGE("%s found two addresses: \t%p, \tRef:%p\n", func_cstr, libc_func, ref_func);

		// should further check dladdr to see if they two come from same library?
		// No, 
        // because correct one may also come from like /usr/lib/system/libsystem_c.dylib
		Dl_info info;
		if (dladdr(libc_func, &info) )  
		{
			LOGE("\tdlsym  from %s\n", info.dli_fname);
		}

		if (dladdr(ref_func, &info) )  
		{
			LOGE("\tdefault from %s\n", info.dli_fname);
		}

        /*
		if ( strcmp ( info.dli_fname, libc_cstr) != 0 )
		{
			return 0; // hook detected
		}
		*/

		return 0; // hook detected
	}

	return libc_func;
}




// init or verify LIBC function pointers
// verify single function pointer
// return: 0-OK, 1-fail/hooked
int init_libc_function_pointers(t_libc_function_pointers*  g_func_pointers)
{
	int isHooked = 0;

	// init function pointers
	memset( (void*) g_func_pointers, 0, sizeof(t_libc_function_pointers));

	// check environment variable
	char* env_value = getenv(libc_env_variable);
	if (env_value)
	{
		LOGE("warning: found env %s=%s\n", libc_env_variable, env_value);
		//isHooked = 1; // in practise should already here return.
	}

	void *handle;
	handle = dlopen(libc_cstr, RTLD_LAZY);  // open up libc directly. // RTLD_NOW
	if ( handle == NULL)
	{
		LOGE("dlopen error: %s\n", dlerror());
		return 0;
	}

	// Locate symbols
	//char *syscalls[] = {"open", "readdir", "fopen", "accept", "access", "unlink", "rand"};
	g_func_pointers->mallocptr = PRELOAD_CHECK(handle, malloc_cstr);
	g_func_pointers->freeptr   = PRELOAD_CHECK(handle, free_cstr);
	g_func_pointers->timeptr   = PRELOAD_CHECK(handle, time_cstr);
	g_func_pointers->strcmpptr = PRELOAD_CHECK(handle, strcmp_cstr);
	g_func_pointers->strstrptr = PRELOAD_CHECK(handle, strstr_cstr);
	g_func_pointers->memcpyptr = PRELOAD_CHECK(handle, memcpy_cstr);   // false positive ??
	g_func_pointers->fgetsptr  = PRELOAD_CHECK(handle, fgets_cstr);
	g_func_pointers->fopenptr  = PRELOAD_CHECK(handle, fopen_cstr);
	g_func_pointers->sscanfptr = PRELOAD_CHECK(handle, sscanf_cstr);
	//g_func_pointers->randptr   = PRELOAD_CHECK(handle, rand_cstr);

	isHooked = isHooked
	           || ! g_func_pointers->mallocptr  || ! g_func_pointers->freeptr
	           || ! g_func_pointers->timeptr    || ! g_func_pointers->strcmpptr
	           || ! g_func_pointers->strstrptr  || ! g_func_pointers->memcpyptr
	           || ! g_func_pointers->fgetsptr   || ! g_func_pointers->fopenptr
	           || ! g_func_pointers->sscanfptr; //  || ! g_func_pointers->randptr;

	LOGE(isHooked ? "Preloaded/Hooked\n" : "OK/no preload\n");

	dlclose(handle);
	return isHooked;
}



#ifdef TEST_LD_PRELOAD

void testrun_function_pointers(t_libc_function_pointers*  g_func_pointers)
{
	// Test 1: Allocate and use memory
	//g_func_pointers.mallocptr = &malloc;
	char *ptr = (* g_func_pointers->mallocptr)(3);
	ptr[0] = 'H'; ptr[1] = 'i'; ptr[2] = '\0';
	printf("test malloc: string=%s\n", ptr);
	// Free it
	(* g_func_pointers->freeptr)(ptr);

	// Test 2:  random
	// check to see if "time()" was replaced.
	printf("test rand: ");
	srand(time(NULL));
	int i = 10;
	while (i--) printf("%d, ", rand() % 100);
	printf("\n");


	// Test 3: fopen
	char hello[] = "hello world";
	FILE *fp = fopen("hello.txt", "w");
	if (fp) {
		fwrite(hello, 1, strlen(hello), fp);
		fclose(fp);
	}
}




int main()
{
	t_libc_function_pointers g_func_pointers;
	init_libc_function_pointers(&g_func_pointers);
	testrun_function_pointers(&g_func_pointers);
	return 0;
}


#endif