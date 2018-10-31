

#include "sepolicy.h"

void init_sepolicy(struct mukey_sepolicy *sepolicy)
{
    sepolicy->enable_jailbreak_check = 1;
    sepolicy->enable_simulator_check = 1;

    sepolicy->enable_jni_hook_detection = 1;
    sepolicy->enable_debugger_check = 0;

    sepolicy->enable_ptrace_deny_attach  = 0;

    sepolicy->enable_appstore_receipt_check  = 1;
    sepolicy->enalbe_image_encryptioninfo_check = 1;
    sepolicy->enalbe_image_codesignature_check = 1;

    sepolicy->enable_dylibs_tampering_check = 0;
    sepolicy->enable_suspicious_dylib_check = 0;
    sepolicy->enable_objcfunc_tampering_check = 0;
    sepolicy->enable_jnifunc_tampering_check  = 1;

    sepolicy->enable_backdoor_process_check = 1;
    sepolicy->enable_backdoor_port_check = 1;

    sepolicy->enable_screenshot_check = 1;
    sepolicy->enable_screenrecord_check = 1;

    sepolicy->enable_library_signature_check = 1;
    sepolicy->enable_ptrace_detection = 0;
    sepolicy->enable_gcc_backtrace_check = 1;
    sepolicy->enable_jav_backtrace_check = 1;
    sepolicy->enable_dex_file_signature_check = 0;  // test
    sepolicy->enable_tracer_pid_check = 1;
    sepolicy->enable_thread_check = 0;
    sepolicy->enable_ld_preload_check = 1;
    sepolicy->enable_system_property_get = 1;
    sepolicy->enable_java_debugger_check = 1;
    sepolicy->enable_loaded_library_validation = 1;

    sepolicy->enable_lkm_check = 1;

    sepolicy->enable_got_hook_check = 1;
    sepolicy->enable_inline_hook_check = 1;
}
