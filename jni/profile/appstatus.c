#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "utility/log.h"
#include "appstatus.h"

// 按测试结果划分
struct mukey_app_status {

  //unsigned char username[128] = "demo";

  uint32_t jailbreak;             // is device jailbroken
  uint32_t simulator;             // is in simulator environment

  uint32_t debugger;              // is gdb or lldb debugger attached
  uint32_t tracerpid;             // check if debugger flag TracerPid is set
  uint32_t ptrace_status;         // is ptrace attached by others
  uint32_t java_debugger;         // is java debugger(jdb, jdwp) attached

  uint32_t appstore_receipt;      // is appstore receipt validated
  uint32_t binary_encryption;     // is my app binary DRM protected
  uint32_t binary_codesign;      // is my app binary signed by developer
  uint32_t dylibs_tampered;       // is any loaded dylib not encrypted or signed
  uint32_t objc_func_tampered;    // is obj-c method injected
  uint32_t jni_func_tampered;     // is any my own JNI function tampered

  uint32_t backdoor_process;      // found any suspicous backdoor process names
  uint32_t backdoor_port;         // found any suspicous backdoor tcp ports

  uint32_t screenshot;            // is screenshot detected
  uint32_t screenrecord;          // is screen recorded, by airplay for example

  uint32_t shared_libraries;      // is loaded shared native libraries validated
  uint32_t xposed_injection;      // either spotted in java backtrace, or by JNI func hook
  uint32_t ld_preload_libs;       // is any LD_PRELOAD library

  uint32_t got_hook;              // is symbol table(GOT) tampered
  uint32_t inline_hook;           // is library tampered (memory mismatch)

  uint32_t gcc_back_trace_status;   // gcc backtrace status

  uint32_t lkm_status;              // is malicious loadable kernel module existing

  // TO be implemented:
  int  lib_tampered;     // anti patch
  unsigned int  lib1_signature; // must read from outside
  int   dex_tampered;
  int   dex_signature;  //  stored dex file CRC value
};

struct mukey_app_status   app_status;




void init_app_status(void)
{
  // default values
}


void set_lkm_status(int val)
{
  app_status.lkm_status = val;
}

void set_jni_func_tampered_status(int val)
{
  app_status.jni_func_tampered = val;
}

void set_gcc_backtrace_status(int val)
{ app_status.gcc_back_trace_status = val; }

void set_xposed_injection_status(int val)
{ app_status.xposed_injection += val;}

void set_tracer_pid_status(int val)
{ app_status.tracerpid = val; }

void set_ptrace_status(int val)
{ app_status.ptrace_status = val; }

void set_ld_preload_status(int val)
{ app_status.ld_preload_libs = val; }


void set_got_hook_status(void)
{ app_status.got_hook = 1;}

void set_inline_hook_status(void)
{ app_status.inline_hook = 1;}


void set_root_jailbreak_status(void)
{ app_status.jailbreak = 1; }

void set_simulator_status(void)
{ app_status.simulator = 1; }

void set_debugger_status(void)
{ app_status.debugger = 1; }

void set_java_debugger_status(int val)
{
  app_status.java_debugger = val;
}

void set_shared_libraries_status(int val)
{
  app_status.shared_libraries = val;
}

void set_appstore_receipt_status(void)
{
  app_status.appstore_receipt = 1;
}

void set_binary_encryption_status(int val)
{
  app_status.binary_encryption = val;
}

void set_binary_codesign_status(int val)
{
  app_status.binary_codesign = val;
}

void set_screenshot_status(int val)
{
  printf("screenshot detected.\n");
  app_status.screenshot = val;
}

void set_screenrecord_status(int val)
{ app_status.screenrecord = val; }

void set_backdoor_port_status(int val)
{
  app_status.backdoor_port = val;
}

void set_backdoor_process_status(int val)
{
  app_status.backdoor_process = val;
}



// 输出结果到一个大字符缓存中
// Usage:
//  char result[5000] = {0};
//  get_app_status(result);

#define eos(s) ((s)+strlen(s))
void get_app_status(char* result, int len)
{
  //char result[6000] = {0};

  overall_check();

  sprintf(eos(result), "app status:\n");
  sprintf(eos(result), "root/jailbreak:     %X\n", app_status.jailbreak);
  sprintf(eos(result), "simulator/emulator: %X\n", app_status.simulator);

  sprintf(eos(result), "gdb/lldb debugger:  %X\n", app_status.debugger);
  sprintf(eos(result), "tracerpid:          %X\n", app_status.tracerpid);
  sprintf(eos(result), "ptrace status:      %X\n", app_status.ptrace_status);
  sprintf(eos(result), "java debugger:      %X\n", app_status.java_debugger);

  sprintf(eos(result), "binary encryption:  %X\n", app_status.binary_encryption);
  sprintf(eos(result), "binary codesign:    %X\n", app_status.binary_codesign);

  sprintf(eos(result), "shared libraries:   %X\n", app_status.shared_libraries);
  sprintf(eos(result), "xposed injected:    %X\n", app_status.xposed_injection);
  sprintf(eos(result), "ld preload:         %X\n", app_status.ld_preload_libs);
  sprintf(eos(result), "gcc backtrace:      %X\n", app_status.gcc_back_trace_status);
  sprintf(eos(result), "lkm rootkits:       %X\n", app_status.lkm_status);
  sprintf(eos(result), "jni function hook:  %X\n", app_status.jni_func_tampered);
  sprintf(eos(result), "symbol table:       %X\n", app_status.got_hook);
  sprintf(eos(result), "inline hook:        %X\n", app_status.inline_hook);

  //
  sprintf(eos(result), "backdoor process:   %X\n", app_status.backdoor_process);
  sprintf(eos(result), "backdoor port:      %X\n", app_status.backdoor_port);

  sprintf(eos(result), "screenshot:         %X\n", app_status.screenshot);
  sprintf(eos(result), "screenrecord:       %X\n", app_status.screenrecord);

  sprintf(eos(result), "library tampered:   %X\n", app_status.dylibs_tampered);
  sprintf(eos(result), "lib tampered:       %X\n", app_status.lib_tampered);
  sprintf(eos(result), "lib1 signature:     %X\n", app_status.lib1_signature);
}
