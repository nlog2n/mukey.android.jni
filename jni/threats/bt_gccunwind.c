#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <unwind.h> // GCC's internal unwinder, part of libgcc
//#include <sys/ucontext.h>
#include <dlfcn.h>

#include "profile/appstatus.h"
#include "utility/log.h"



// same as used in bt_execinfo.c
int check_addr(void * addr)
{
  // the address could come from num= backtrace(buffer,size)
  Dl_info info;
  if (!dladdr(addr, &info))
    return 0;   // I cannot tell

  // library name
  if (info.dli_fname)
  {
    printf("(lib=%s", info.dli_fname);
    // (char*) addr - (char*) info.dli_fbase

    // blacklist = { "frida", "cycript"}
    if ( strstr(info.dli_fname, "frida") != NULL
      || strstr(info.dli_fname, "cycript") != NULL )
    {
      LOGE("found hooker in %s!\n", info.dli_fname);
      return  1; // return directly

      // or set global status as hooked
    }

    // whitelist check: the library must in given list, using strncmp and strcmp.
  }

  // symbol/function name
  if (info.dli_sname)
  {
    printf(",sym=%s)\n", info.dli_sname);

    if (   strstr(info.dli_sname, "frida") != NULL
        || strstr(info.dli_sname, "cycript") != NULL )
    {
      LOGE("found hooker in %s!\n", info.dli_sname);
      return 1; // return directly
    }
  }

  return 0;  // nothing happened
}




// implementation of callback function
static _Unwind_Reason_Code trace_fcn( struct _Unwind_Context *ctx, void *d)
{
  int *depth = (int*)d;

  // get function address
  char* addr = (char*)_Unwind_GetIP(ctx);
  printf("#%d: 0x%08x", *depth, addr);
  (*depth)++;

  if ( *depth > 50 )  // set max depth
    return   _URC_END_OF_STACK; //_URC_NORMAL_STOP;
   // _URC_NORMAL_STOP is not defined in recent arm toolchains
   // refer to: https://github.com/MainRo/edKit/issues/10
   // possible solution:
   // We just want to stop the walk, so any error code will do.
   //           Using _URC_NORMAL_STOP would probably be the most accurate,
   //          but it is not defined on Android for ARM.
   //       return _URC_FOREIGN_EXCEPTION_CAUGHT;


  // can we get extra info about library?
  if (check_addr(addr))
  {
    // found hooker, and no more tracing
    // set status in returned depth
    *depth = -1;
    return  _URC_END_OF_STACK; // _URC_NORMAL_STOP; // return directly, or maybe _URC_END_OF_STACK
  }

  return _URC_NO_REASON;
}


// return:  0 - OK,  1 - found hooker
int validate_backtrace()
{
  int depth = 0;
  _Unwind_Backtrace(&trace_fcn, &depth); // assign callback

  // reuse the depth to return status
  depth = ( depth == -1 ? 1 :0 );
  return depth;
}




///////////////////////////////////// test code


#ifdef  TEST_BACKTRACE

/* "static" means don't export the symbol... */
static void  myfunc2(void) { validate_backtrace(); }

// recursive function, to show backtrace in diff depth
void  myfunc(int ncalls)
{
  if (ncalls > 1)
    myfunc(ncalls - 1);
  else
    myfunc2();
}



double mean(double a, double b)
{
  if (validate_backtrace() ) return 0;
  else  return (a + b) / 2;
}


int    main(int argc, char *argv[])
{

  if (argc != 2) {
    fprintf(stderr, "%s num-calls\n", argv[0]);
    exit(EXIT_FAILURE);
  }

  myfunc(atoi(argv[1]));   // check 1

  double v1, v2, m;
  v1 = 5.2;
  v2 = 7.9;

  m  = mean(v1, v2);    // check 2
  printf("The mean of %3.2f and %3.2f is %3.2f\n", v1, v2, m);

  exit(EXIT_SUCCESS);
}

#endif
