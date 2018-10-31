#include <stdio.h>

#include "api/muapi.h"
#include "utility/log.h"

int overall_check(void)
{
    struct mukey_sepolicy   sepolicy;
    init_sepolicy(&sepolicy);

    int status = 0;
    printf("started...\n");

    // test android device information get
    if (sepolicy.enable_system_property_get)
    {
        android_device_info device;
        read_android_device_info(&device);
        print_android_device_info(&device);
        generate_device_id(&device);

        status = detect_emulator(&device);
        if (status)
        {
           set_simulator_status();
        }
    }

    if (sepolicy.enable_jailbreak_check)
    {
       is_device_rooted();
       check_su_files();
       status = (check_system_properties() == 1);
       if (status)
       {
         set_root_jailbreak_status();
       }
    }


    // test tracerpid to see any debugger attached
    if (sepolicy.enable_tracer_pid_check)
    {
        status = check_tracerpid();
        LOGE("tracerpid status =%d", status );
        set_tracer_pid_status(status);

        //anti_debug2();
    }

    // test ptrace deny protection. warning: this will deny adb debugger
    if (sepolicy.enable_ptrace_deny_attach)
    {
        // do not use these two methods at the same time!
    }
    if (sepolicy.enable_ptrace_detection)
    {
        status = ptrace_check();
        set_ptrace_status(status);
    }


    // check jdb attach
    if (sepolicy.enable_java_debugger_check)
    {
        status = is_jdwp_exist();
        set_java_debugger_status(status);
    }

    // test gcc backtrace validation
    if (sepolicy.enable_gcc_backtrace_check)
    {
        status = validate_backtrace();
        set_gcc_backtrace_status(status);
    }

    // test java call trace validation
    if (sepolicy.enable_jav_backtrace_check)
    {
        status = validate_java_calltrace();
        set_xposed_injection_status(status);
    }

    //  test JNI hook validation - anti xposed
    if (sepolicy.enable_jni_hook_detection)
    {
        // a general sample
        status = validate_JNI_hook2();
        set_xposed_injection_status(status);
    }

    if (sepolicy.enable_loaded_library_validation)
    {
        status = validate_loaded_library();
        set_shared_libraries_status(status);
    }

    // test if any LD_PRELOAD library defined
    if (sepolicy.enable_ld_preload_check)
    {
        t_libc_function_pointers g_func_pointers;
        status = init_libc_function_pointers(&g_func_pointers);
        set_ld_preload_status(status);
    }

    if (sepolicy.enable_lkm_check)
    {
        check_kernel_modules();
    }

    if (sepolicy.enable_backdoor_port_check)
    {
      scan_port_files();
    }

    if (sepolicy.enable_backdoor_process_check)
    {
      scan_processes();
    }

    if (sepolicy.enable_jnifunc_tampering_check)
    {
      validate_func_ptr();
    }

    if (sepolicy.enable_got_hook_check)
    {
      check_got_hook();
    }

    if (sepolicy.enable_inline_hook_check)
    {
      check_inline_hook();
    }



// TODO:
    // test android so library signature
    if (sepolicy.enable_library_signature_check)
    {
        validate_signed_library2();
    }

    // test dex file signature
    // bug: 该项检查打开后，启动应用，然后直接点击"App Status",会异常退出
    //    但是点击其他比如"Trusted Services"后再点击"App Status", 则OK
    // 怀疑是传入的Context is Null造成.
    if (sepolicy.enable_dex_file_signature_check)
    {
        compute_dexfile_signature();
        load_dexfile_signature();
        //validate_dexfile_signature();
    }



    return status;
}
