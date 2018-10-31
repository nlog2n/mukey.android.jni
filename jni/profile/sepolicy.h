#ifndef __MUKEY_SEPOLICY_H__
#define __MUKEY_SEPOLICY_H__

#include <stdlib.h>

// 按功能(security feature)划分

struct mukey_sepolicy {
    uint32_t  enable_jailbreak_check;               // ios jailbreak / android root
    uint32_t  enable_simulator_check;               // ios simulator / android emulator

    uint32_t  enable_debugger_check;                // ios gdb, lldb / android gdb
    uint32_t  enable_ptrace_deny_attach;            // ios
    uint32_t  enable_ptrace_detection;              // android

    uint32_t  enable_appstore_receipt_check;        // ios only

    uint32_t  enalbe_image_encryptioninfo_check;    // ios
    uint32_t  enalbe_image_codesignature_check;     // ios
    uint32_t  enable_dylibs_tampering_check;        // ios
    uint32_t  enable_suspicious_dylib_check;        // ios
    uint32_t  enable_objcfunc_tampering_check;      // ios

    uint32_t  enable_jnifunc_tampering_check;          // android

    uint32_t  enable_backdoor_process_check;        // ios and android
    uint32_t  enable_backdoor_port_check;           // ios and android

    uint32_t  enable_screenshot_check;              // ios
    uint32_t  enable_screenrecord_check;            // ios

    // android
    uint32_t  enable_jni_hook_detection;            // android only
    uint32_t  enable_library_signature_check;       // android
    uint32_t  enable_gcc_backtrace_check;           // android
    uint32_t  enable_jav_backtrace_check;           // android only
    uint32_t  enable_dex_file_signature_check;      // android only
    uint32_t  enable_tracer_pid_check;              // android only
    uint32_t  enable_thread_check;                  // android
    uint32_t  enable_ld_preload_check;              // android
    uint32_t  enable_system_property_get;           // android
    uint32_t  enable_java_debugger_check;           // android only: jdwp, jdb debugger
    uint32_t  enable_loaded_library_validation;     // android

    uint32_t  enable_lkm_check;                     // android

    uint32_t  enable_got_hook_check;                // android
    uint32_t  enable_inline_hook_check;             // android

};



#ifdef __cplusplus
extern "C" {
#endif

    void init_sepolicy(struct mukey_sepolicy *sepolicy);

#ifdef __cplusplus
}
#endif

#endif
