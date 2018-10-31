#ifndef __MUKEY_FUNC_VALIDATE_H__
#define __MUKEY_FUNC_VALIDATE_H__

#include <string.h>

#define proc_pid_maps_cstr                       "/proc/%d/maps"

#ifdef __cplusplus
extern "C" {
#endif



   int is_funcaddr_valid(void *func, const char* library_name);
   size_t get_loaded_lib_location(const char* lib_name, size_t *start, size_t *end);



#ifdef __cplusplus
}
#endif


#endif /* __MUKEY_FUNC_VALIDATE_H__ */
