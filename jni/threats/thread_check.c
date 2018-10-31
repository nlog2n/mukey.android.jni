
// put some checks inside a thread

#include <stdio.h>
#include <string.h>

#include <sys/stat.h>
#include <fcntl.h>
#include <signal.h>  //for the SIGKILL
#include <pthread.h>  // for pthread

#include "tracerpid_check.h"
#include "ptrace_check.h"

#include "utility/log.h"


// The entire lifetime of an activity happens between the first call
// to onCreate() through to a single final call to onDestroy(). An
// activity does all its initial setup of "global" state in onCreate(),
// and releases all remaining resources in onDestroy(). For example, if
// it has a thread running in the background to download data from the
// network, it may create that thread in onCreate() and then stop the
// thread in onDestroy().

// So, anything created in or after onCreate() should be discarded in or
// before onDestroy().


int keepAlive = 1;

// check every second
void *thread_task(void *x_void_ptr)
{
    while (keepAlive)
    {
        LOGI("start thread check...");

        check_tracerpid();

        ptrace_check();

            // TODO: create more checks here

        LOGI("stop thread check...");

        sleep(1);

        LOGI("thread then wake up...");
    }

    /* the function must return something - NULL will do */
    return NULL;
}


// can put inside Jni_Onload at the beginning
void launch_thread_check()
{
    pthread_t tid;
    if(pthread_create(&tid, NULL, thread_task, NULL))
    {
       LOGE("Error creating thread\n");
       return ;
    }
}



#ifdef  TEST_THREAD_CHECK

int main()
{
	launch_thread_check();

	sleep(120);

    return 0;
}

#endif

