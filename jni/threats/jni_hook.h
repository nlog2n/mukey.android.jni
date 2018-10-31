#ifndef __JNI_HOOK_H__
#define __JNI_HOOK_H__


#include <dlfcn.h>
#include <fcntl.h>
#include <sys/mman.h>

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>

#define libdvm_cstr                          "libdvm.so"

#define framework_java_classes_dex_cstr      "/cache/dalvik-cache/system@framework@framework.jar@classes.dex"  // inaccessible
//#define framework_odex_cstr                  "/system/framework/framework.odex"   // obsolete
#define framework_odex_cstr                  "/system/framework/framework.jar"      // at least works for android 4.4


#define fanghui_java_class_name              "Lcom/nlog2n/mukey/MainActivity;"
#define fanghui_java_func_name               "returnPwd"
#define fanghui_java_func_params             "(Ljava/lang/String;)Ljava/lang/String;"


#define Setting_Secure_cstr                  "Landroid/provider/Settings$Secure;"
#define getString_cstr                       "getString"
#define getSetringParam_cstr                 "(Landroid/content/ContentResolver;Ljava/lang/String;)Ljava/lang/String;"

#define TelephonyManager_cstr                "Landroid/telephony/TelephonyManager;"
#define getDeviceId_cstr                     "getDeviceId"
#define getDeviceIdParam_cstr                "()Ljava/lang/String;"

#define _Z17dvmIsNativeMethodPK6Method_cstr                               "_Z17dvmIsNativeMethodPK6Method"
#define _Z18dvmFindLoadedClassPKc_cstr                                    "_Z18dvmFindLoadedClassPKc"
#define _Z31dvmFindDirectMethodByDescriptorPK11ClassObjectPKcS3_cstr      "_Z31dvmFindDirectMethodByDescriptorPK11ClassObjectPKcS3_"
#define _Z36dvmFindVirtualMethodHierByDescriptorPK11ClassObjectPKcS3_cstr "_Z36dvmFindVirtualMethodHierByDescriptorPK11ClassObjectPKcS3_"

#define dvmFindDirectMethodByDescriptor_cstr                              "dvmFindDirectMethodByDescriptor"
#define dvmFindLoadedClass_cstr                                           "dvmFindLoadedClass"
#define dvmFindVirtualMethodHierByDescriptor_cstr                         "dvmFindVirtualMethodHierByDescriptor"


//  not yet in use
#define elapsedRealtime_cstr                         "_ZN7android15elapsedRealtimeEv"
#define dvmDbgIsDebuggerConnected_cstr               "dvmDbgIsDebuggerConnected"
#define _ZN3art3Dbg16IsDebuggerActiveEv_cstr         "_ZN3art3Dbg16IsDebuggerActiveEv"
#define _Z25dvmDbgIsDebuggerConnectedv_cstr          "_Z25dvmDbgIsDebuggerConnectedv"
//


typedef unsigned char       u1;
typedef unsigned short      u2;
typedef unsigned int        u4;
typedef unsigned long long  u8;
typedef signed char         s1;
typedef signed short        s2;
typedef signed int          s4;
typedef signed long long    s8;


typedef void (*DalvikBridgeFunc)(const u4* args, void* pResult, const void* method, void* self);

typedef struct DexProto {
	u4* dexFile;
	u4 protoIdx;
} DexProto;

typedef struct Method {
	struct ClassObject *clazz;
	u4 				a;
	u2             	methodIndex;
	u2              registersSize;
	u2              outsSize;
	u2              insSize;
	const char*     name;
	DexProto        prototype;
	const char*     shorty;
	const u2*       insns;
	int             jniArgInfo;
	DalvikBridgeFunc  nativeFunc;

#ifdef WITH_PROFILER
	bool            inProfile;
#endif
#ifdef WITH_DEBUGGER
	short           debugBreakpointCount;
#endif

	bool fastJni;
	bool noRef;
} Method;

typedef void* (*dvmFindVirtualMethodHierByDescriptor_func)(void*, const char*, const char*);
typedef void* (*dvmFindDirectMethodByDescriptor_func)(void*, const char*, const char*);
typedef void* (*dvmFindLoadedClass_func)(const char*);
typedef void* (*dvmIsNativeMethod_func)(void*);

struct dexstuff_t
{
	void *dvm_hand;
	dvmFindVirtualMethodHierByDescriptor_func dvmFindVirtualMethodHierByDescriptor_fnPtr;
	dvmFindDirectMethodByDescriptor_func dvmFindDirectMethodByDescriptor_fnPtr;
	dvmFindLoadedClass_func dvmFindLoadedClass_fnPtr;
	dvmIsNativeMethod_func dvmIsNativeMethod_fnPtr;
	void *gDvm;
};




#ifdef __cplusplus
extern "C" {
#endif

int validate_JNI_hook(char *cls, char *mth, char *param, int is_dalvik_func);

int validate_JNI_hook2();


#ifdef __cplusplus
}
#endif


#endif
