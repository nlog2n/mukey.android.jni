#ifndef __MUKEY_ANDROID_API_H__
#define __MUKEY_ANDROID_API_H__

#include "device/deviceid.h"
#include "profile/sepolicy.h"
#include "profile/appstatus.h"

// root detection
#include "threats/check_sysfiles.h"
#include "threats/check_suid.h"

// backdoor processes
#include "threats/proc_scan.h"
// backdoor ports
#include "threats/port_scan.h"

// anti debug
#include "threats/ptrace_check.h"
#include "threats/tracerpid_check.h"
#include "threats/thread_check.h"
// anti jdb debug
#include "threats/jdwp_check.h"

// anti hook
#include "threats/bt_gccunwind.h"
#include "threats/ld_preload.h"

// anti tampering
#include "threats/anti_patch.h"

// JNI related
#include "utility/jnienv.h"
#include "threats/jni_hook.h"
#include "threats/jni_calltrace.h"
#include "threats/jni_dex_sign.h"

// library check
#include "threats/validate_loaded_library.h"

#include "threats/lkm_check.h"

// got and inline hook check
#include "threats/validate_inline_got_hook.h"


#ifdef __cplusplus
extern "C" {
#endif

   //int  getKey(const unsigned char *buffer);  // mukey service

   int overall_check(void);


#ifdef __cplusplus
}
#endif

#endif
