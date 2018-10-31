#ifndef __MUKEY_APP_STATUS_H__
#define __MUKEY_APP_STATUS_H__

#include <stdlib.h>

#ifdef __cplusplus
extern "C" {
#endif


void set_xposed_injection_status(int val);
void set_tracer_pid_status(int val);
void set_ptrace_status(int val);

void set_gcc_backtrace_status(int val);
void set_jni_func_tampered_status(int val);

void set_ld_preload_status(int val);
void set_lkm_status(int val);

void set_got_hook_status(void);
void set_inline_hook_status(void);

void set_root_jailbreak_status(void);
void set_simulator_status(void);

void set_debugger_status(void);
void set_java_debugger_status(int val);

void set_shared_libraries_status(int val);

void set_appstore_receipt_status(void);

void set_binary_encryption_status(int val);
void set_binary_codesign_status(int val);

void set_screenshot_status(int val);
void set_screenrecord_status(int val);

void set_backdoor_port_status(int val);
void set_backdoor_process_status(int val);


void get_app_status(char* result, int len);



#ifdef __cplusplus
}
#endif

#endif
