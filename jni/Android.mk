LOCAL_PATH:= $(call my-dir)

######################################################## Utility
include $(CLEAR_VARS)

LOCAL_MODULE    := utility
LOCAL_CFLAGS    := -Werror
#LOCAL_LDLIBS    := -llog
LOCAL_SRC_FILES := \
	utility/property_get.c \
	utility/proc_pid_maps.c \
	utility/pid2cmdline.c \
	utility/popen.c \
	utility/jnienv.c \
	utility/fileio.c \

include $(BUILD_STATIC_LIBRARY)



######################################################## DeviceId
include $(CLEAR_VARS)

LOCAL_MODULE    := deviceid
LOCAL_CFLAGS    := -Werror
#LOCAL_LDLIBS    := -llog
LOCAL_STATIC_LIBRARIES := utility
LOCAL_SRC_FILES := \
	device/android_cid.c \
	device/android_cpuinfo.c \
	device/android_imei.c \
	device/android_macaddr_bt.c \
	device/android_macaddr_wifi.c \
	device/android_sys_info.c \
	device/deviceid.c \
	device/jni_androidid.c \
	device/jni_btmac.c \
	device/jni_imei.c \
	device/jni_wifimac.c \
	device/check_emulator.c \


include $(BUILD_STATIC_LIBRARY)



######################################################## Threats
include $(CLEAR_VARS)

LOCAL_MODULE    := threats
LOCAL_CFLAGS    := -pthread -fexceptions -frtti -lgccunwind -DANDROID -D_GNU_SOURCE #-std=c++11 #-Werror
#LOCAL_LDLIBS    := -llog -lz -landroid -latomic
LOCAL_STATIC_LIBRARIES := utility
LOCAL_SRC_FILES := \
	threats/ptrace_check.c \
	threats/thread_check.c \
	threats/tracerpid_check.c \
	threats/gdb_check.c \
	threats/anti_patch.c \
	threats/bt_gccunwind.c \
	threats/jni_dex_sign.c \
	threats/dexopt.c \
	threats/jni_calltrace.c \
	threats/jni_hook.c \
	threats/ld_preload.c \
	threats/lkm_check.c \
	threats/jdwp_check.c \
	threats/port_scan.c \
	threats/proc_scan.c \
	threats/check_sysfiles.c \
	threats/check_suid.c \
	threats/validate_loaded_library.c \
	threats/validate_inline_got_hook.c \
	threats/validate_func_ptr.c \


include $(BUILD_STATIC_LIBRARY)


########################################################## MUKEY
include $(CLEAR_VARS)

LOCAL_MODULE    := mukey
LOCAL_CFLAGS    := -Werror
LOCAL_LDLIBS    := -llog -lz -landroid -latomic

LOCAL_STATIC_LIBRARIES := utility deviceid threats

#LOCAL_C_INCLUDES += mukey/app/src/main/jni

LOCAL_SRC_FILES := \
	jniwrapper.cpp \
	profile/appstatus.c \
	profile/sepolicy.c \
	api/muapi.c \

include $(BUILD_SHARED_LIBRARY)
