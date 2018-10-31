
// C code to check if debugger is attached to myself
// only works under linux, probably

// use "ps axf" to view which process is debugging which process.


#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <fcntl.h>    // for O_RDONLY (we use syscall to do actual reads)

#include "utility/log.h"


int initial_tracer_pid = 0;  // should record Zygote process id in Java_OnLoad()


// func: If current process is being ptrace()d, 'TracerPid' in /proc/self/status
//       will be non-zero.
// return:  0 - OK, others - detected
int check_tracerpid(void)
{
    int debugger_present = 0;

    char buf[1024] = {0};  // TracerPid shows up in the front of status file

    // get path to status file
    int my_pid = getpid();
    char filename[64] = {0};
    sprintf(filename, "/proc/%d/status", my_pid);  // in linux can also check as "/proc/self/status"

    // read file into a buffer
    int status_fd = open(filename, O_RDONLY);
    if (status_fd == -1)
    {
        //LOGE("tracerpid check: file %s not found", filename);
        return 0;  // cannot tell for sure
    }
    ssize_t num_read = read(status_fd, buf, sizeof(buf));
    //LOGI("read %d bytes.\n", (int)num_read);

    if (num_read > 0)
    {
        static const char TracerPid[] = "TracerPid:";
        char *tracer_pid_str;

        buf[num_read] = 0;  // make sure it is a string
        tracer_pid_str    = strstr(buf, TracerPid);
        if (tracer_pid_str)  // get to the right place
        {
			      //printf("found TracerPid at %s\n", tracer_pid_str);
            int tracer_pid = atoi(tracer_pid_str + sizeof(TracerPid) - 1);
            LOGI( "current tracerPID=%d, initial tracerPID=%d\n", tracer_pid, initial_tracer_pid);

            // for Android, non-zero tracer pid could be zygote, which is normal.
            // so one way is to intially save zygote pid, and then check if
            // there is any change: changed tracerPID means debugger is attaching
            debugger_present =  (tracer_pid != 0) && (tracer_pid != initial_tracer_pid);
            if ( debugger_present)
            {
				       //LOGE("i am debugged!\n");
			   	     //int ret = kill(my_pid, SIGKILL);
				       // SCRAMBLE_GSTATUS();
			      }
        }

    }

    close(status_fd);
    return debugger_present;
}
