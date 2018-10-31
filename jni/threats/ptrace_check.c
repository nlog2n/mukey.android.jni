#include <sys/ptrace.h>
#include <unistd.h>
#include <stdio.h>

// A simply deny other debugger to attach since I did it myself in advance
// too simple we do not use it.
/*
void ptrace_deny()
{
  ptrace(PTRACE_TRACEME, 0, 0, 0);
}
*/



//////////////////// debugger check method 1: ptrace
// note: this function may be called initially and later spammed into everywhere.
int ptrace_check()
{
#define  PTRACE_MY_SEAT_NUMBER     0x35919513   // magic number

  static int ptrace_flag = 0;  // initially there is no debugger attached.

	if(ptrace(PTRACE_TRACEME, 0, 0, 0)==0)
	{
	   // so far no gdb debugger attached, so I take the seat first successfully.
		ptrace_flag = PTRACE_MY_SEAT_NUMBER;
	}
	else   // found attachment, may be by me or gdb..
	{
		if(ptrace_flag !=  PTRACE_MY_SEAT_NUMBER)   // unfortunately i did not take that seat
		{
		    // so should be someone else
				return 1;
			//try to kill the debugger
			//ptrace(31, 0, 0, 0);
		}
	}

	return 0;
}
