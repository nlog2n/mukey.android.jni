#include "jni_hook.h"
#include "profile/appstatus.h"
#include "utility/log.h"



// introduced from anti_patch.c
size_t get_loaded_lib_location(const char* lib_name, size_t *start, size_t *end);



// to resist Xposed hooking
// input:  class name, method name, and method parameters
//           is_dalvik_func  - this java method belongs to dvm framework function (from jar)
// return:   0  OK
//           positive values    OK with some issues
//           negative values    found xposed
int validate_JNI_hook(char *cls, char *mth, char *param, int is_dalvik_func)
{
	int result = 0;

	static struct dexstuff_t dex;
	dex.dvm_hand = dlopen(libdvm_cstr, RTLD_NOW);
	if ( !dex.dvm_hand )
	{
		return 1;  // dlopen error
	}

	// search method for finding loaded class
	dex.dvmFindLoadedClass_fnPtr = (dvmFindLoadedClass_func) dlsym(dex.dvm_hand, _Z18dvmFindLoadedClassPKc_cstr);
	if (!dex.dvmFindLoadedClass_fnPtr) {
		dex.dvmFindLoadedClass_fnPtr = (dvmFindLoadedClass_func) dlsym(dex.dvm_hand, dvmFindLoadedClass_cstr);
	}

	// search method for finding direct method by descirptor
	dex.dvmFindDirectMethodByDescriptor_fnPtr = (dvmFindDirectMethodByDescriptor_func)
	        dlsym(dex.dvm_hand, _Z31dvmFindDirectMethodByDescriptorPK11ClassObjectPKcS3_cstr);
	if (!dex.dvmFindDirectMethodByDescriptor_fnPtr) {
		dex.dvmFindDirectMethodByDescriptor_fnPtr = (dvmFindDirectMethodByDescriptor_func)
		        dlsym(dex.dvm_hand, dvmFindDirectMethodByDescriptor_cstr);
	}

	// search method for finding virtual method by descriptor
	dex.dvmFindVirtualMethodHierByDescriptor_fnPtr = (dvmFindVirtualMethodHierByDescriptor_func)
	        dlsym(dex.dvm_hand, _Z36dvmFindVirtualMethodHierByDescriptorPK11ClassObjectPKcS3_cstr);
	if (!dex.dvmFindVirtualMethodHierByDescriptor_fnPtr)
	{
		dex.dvmFindVirtualMethodHierByDescriptor_fnPtr = (dvmFindVirtualMethodHierByDescriptor_func)
		        dlsym(dex.dvm_hand, dvmFindVirtualMethodHierByDescriptor_cstr);
	}

	// search method find identifying if it is native method
	dex.dvmIsNativeMethod_fnPtr = (dvmIsNativeMethod_func) dlsym(dex.dvm_hand, _Z17dvmIsNativeMethodPK6Method_cstr);


	// now call these java methods to pinpoint the interested java class method
	if ( !dex.dvmFindLoadedClass_fnPtr )
	{
		dlclose(dex.dvm_hand);
		return 2;      // no handler for finding class
	}
	void *target_cls = dex.dvmFindLoadedClass_fnPtr(cls);
	if ( !target_cls ) {
		dlclose(dex.dvm_hand);
		return 3;   // did not find java class
	}

	if ( !dex.dvmFindDirectMethodByDescriptor_fnPtr ) {
		dlclose(dex.dvm_hand);
		return 4;   // no handler for finding method
	}
	Method *method = (Method*) dex.dvmFindDirectMethodByDescriptor_fnPtr(target_cls, mth, param);
	if (method == NULL) {
		if ( !dex.dvmFindVirtualMethodHierByDescriptor_fnPtr ) {
			dlclose(dex.dvm_hand);
			return 5; // no handler for finding method
		}

		method = (Method*) dex.dvmFindVirtualMethodHierByDescriptor_fnPtr(target_cls, mth, param);
	}
	if ( !method ) {
		dlclose(dex.dvm_hand);
		return 6;   // did not find java method
	}


	// now we got java method
	//DEBUG_BASIC_PRINT("Results: location %p native_func %p insns %p function %s\n",
	//		method, method->nativeFunc, method->insns, method->name);


	// check if the Java function is re-registerd as a Native method
	// check for (dex.dvmIsNativeMethod_fnPtr == NULL) to cater Android 2.6
	if ((dex.dvmIsNativeMethod_fnPtr == NULL) || dex.dvmIsNativeMethod_fnPtr(method))
	{
		// debug: 奇怪在 Android 4.2.2 emulator上 dex.dvmIsNativeMethod_fnPtr(method)返回真.
		if (dex.dvmIsNativeMethod_fnPtr == NULL)
		{
			LOGE("dvmIsNativeMethod_fnPtr == NULL");
		}

		// normal java method will be re-registered by Xposed as JNI native method.
		// so if reaching here, we can assume Xposed has stepped in.
		result = -1; // this normal java method was registered by xposed as jni method

		// further check if the method lies between specified memory range
		if ( is_dalvik_func )
		{
		long start, end;
		get_loaded_lib_location(framework_java_classes_dex_cstr, &start, &end);
		if (start == 0) {
			get_loaded_lib_location(framework_odex_cstr, &start, &end);
			if ( start == 0)
			{
				dlclose(dex.dvm_hand);
				return -2;  // framework not found
			}
		}

		// determine where 'method->insns' points to
		if (!(((long)method->insns > start) && ((long)method->insns < end)))
		{
			//DEBUG_BASIC_PRINT("Results: JNI HOOK FOUND. affected class: %s method: %s address: %p\n", cls, mth, method->insns);
			result = -3;
		}
		}
	}

	dlclose(dex.dvm_hand);

	return result;
}


// api
// return: 0 - OK, others - xposed injected
int validate_JNI_hook2()
{
	int result = 0;

	// java system method: getString
   int status = validate_JNI_hook(Setting_Secure_cstr, getString_cstr, getSetringParam_cstr, 1);
	LOGE( "hook check getString status = %d", status );
   if ( status < 0 )
	  {
			result += 2;
		}

		// java system method: getDeviceId
   status = validate_JNI_hook(TelephonyManager_cstr, getDeviceId_cstr, getDeviceIdParam_cstr, 1);
		LOGE( "hook check getDeviceID status =%d", status );
   if ( status < 0 )
	 {
     result += 4;
	 }


	 // user defined java method:
   status = validate_JNI_hook(fanghui_java_class_name, fanghui_java_func_name, fanghui_java_func_params, 0);
 	LOGE("hook check returnPwd status =%d", status );
    if ( status < 0 )
		{
			result += 8;
		}

		return result;
}



// 之前是给JNI hook detection 用，如果是ART runtime则不用做jni hook detection
#include <sys/system_properties.h>
#define persist_sys_dalvik_vm_lib_2_cstr     "persist.sys.dalvik.vm.lib.2"
#define libart_cstr                          "libart"
int is_art_runtime()
{
	int isARTRuntime = 0;
	char getprop[20];

	if (__system_property_get(persist_sys_dalvik_vm_lib_2_cstr, getprop))
	{
		if (strncmp(libart_cstr, getprop, 6) == 0) {
			isARTRuntime = 1;
		}
	}

	return isARTRuntime;
}



#ifdef TEST_XPOSED_HOOK

int main(int argc, const char *argv[])
{
validate_JNI_hook(Setting_Secure_cstr, getString_cstr, getSetringParam_cstr, 1);

validate_JNI_hook(TelephonyManager_cstr, getDeviceId_cstr, getDeviceIdParam_cstr, 1);

return 0;

}

#endif
