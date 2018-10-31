#ifndef __MU_PID2CMDLINE_H__
#define __MU_PID2CMDLINE_H__



#ifdef __cplusplus
extern "C" {
#endif


int get_name_by_pid_cmdline(int pid, char* appname);
int get_pid_by_name_cmdline(const char* appname);


#ifdef __cplusplus
}
#endif


#endif
