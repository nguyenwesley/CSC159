#include "spede.h"      // for IO_DELAY() needed here below
#include "extern.h"     // for current_run_pid needed here below
#include "proc.h"       // for prototypes of process functions
#include "syscall.h"    // for system calls

// IO_DELAY() should be encapsulated in a loop 1666000 times
// However, IO_DELAY() delays CPU by 0.65us
//   -> Therefore, we should loop 1s/0.65us = 1538461.53846 -> 1538462
#define DELAY_SECOND 1538462

void InitProc() {
    int i;

    while (1) {
        cons_printf("Init runs\n");

        for (i = 0; i < DELAY_SECOND; i++) {
            IO_DELAY();
        }
    }
}

void UserProc() {
    // Display the PID, time, and put this process to sleep
    // Add some variation for the sleep time:
    //   sleep_time = pid % 5 + 1
	while (1)
	{
		int pid = GetPid();
		int time = GetTime();
		cons_printf("PID: %d\t\tSystem Time: %d\t\tSleep Length: %d\n", pid, time, (pid % 5 + 1));
		Sleep(pid % 5 + 1);
	}
}
