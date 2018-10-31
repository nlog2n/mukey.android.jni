#ifndef __MU_GDB_CHECK_H__
#define __MU_GDB_CHECK_H__



#ifdef __cplusplus
extern "C" {
#endif


int is_ppid_gdb_debugger();
int find_android_server();
int check_proc_task_status();


#ifdef __cplusplus
}
#endif


#endif
