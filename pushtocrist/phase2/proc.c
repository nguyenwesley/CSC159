#include "spede.h"      // for IO_DELAY() needed here below
#include "extern.h"     // for current_run_pid needed here below
#include "proc.h"       // for prototypes of process functions
#include "syscall.h"    // for system calls

// IO_DELAY() should be encapsulated in a loop 1666000 times
// However, IO_DELAY() delays CPU by 0.65us
//   -> Therefore, we should loop 1s/0.65us = 1538461.53846 -> 1538462
#define DELAY_SECOND 1538462


//Modified for phase2
void InitProc() {
    int i;
	/* 
	*InitProc will be modified to:
	*X	Display a message that it is starting (before the run loop)
	*X	Remove printing within the run loop
	*/
	
	cons_printf("Init runs\n");
    while (1) {

        for (i = 0; i < DELAY_SECOND; i++) {
            IO_DELAY();
        }
    }
}



void UserProc() {
	/*
	*UserProc will be modified to:
    *X	Display a message that it is starting (before the run loop)
    *X	In the run loop:
	*X		Send a message (the current system time) to mailbox 1

	*The existing sleep behavior will be retained for each user process
	*/
	msg_t toSend;
	int mboxNum = 1;
	int pid = GetPid();
	cons_printf("User runs\n");
	while (1)
	{
		toSend.sender = GetPid();
		toSend.time_stamp = GetTime();
		toSend.data = toSend.time_stamp;
		
		MsgSend(mboxNum, &toSend);
		cons_printf("PID: %d UserProc sending %d to mailbox %d @ %d\n", 
					toSend.sender, toSend.data, mboxNum, toSend.time_stamp);
		Sleep(pid % 5 + 1);
	}
}


//Phase2 Procs

void PrinterProc() {
	/*
	*PrinterProc is a new process that will:
    *X	Display a message that it is starting (before the run loop)
    *X	Initialize (get) the semaphore if it has not yet been initialized
    *	In the run loop:
    *		Read from shared memory (a global integer)
    *		Print to the console
    *X		Sleep for 5 seconds
	*/
	cons_printf("Printer runs\n");
	if (semGetID == -1)
	{
		semGetID = SemGet();
	}
	
	while (1)
	{
		int pid = GetPid();
		int time = GetTime();
		int toPrint;
		SemWait(semGetID);
		toPrint = critSec;
		SemPost(semGetID);
		cons_printf("PID: %d Printer read shared memory %d @ %d\n", 
					pid, toPrint, time);
		Sleep(5);
	}
}


void DispatcherProc() {
	/*
	*DispatcherProc is a new process that will:
    *X	Display a message that it is starting (before the run loop)
	*X	Initialize (get) the semaphore if it has not yet been initialized
	*	In the run loop:
	*		Receive a message from mailbox 1
	*		Write the value received from the mailbox to the shared memory
	*		Print a message to the console that indicates what it wrote to the shared memory
	*		No sleep is required (it should block/wait for a message to be received)
	*/
	msg_t toRecv;
	int mboxNum = 1;
	cons_printf("Dispatcher runs\n");
	if (semGetID == -1)
	{
		semGetID = SemGet();
	}
	
	while (1)
	{
		int time;
		int pid = GetPid();
		MsgRecv(mboxNum, &toRecv);
		time = GetTime();
		cons_printf("PID: %d Dispatcher received %d from PID %d in mbox %d @ %d\n", 
					pid, toRecv.data, toRecv.sender, mboxNum, time);
		SemWait(semGetID);
		critSec = toRecv.data;
		cons_printf("Dispatcher setting shared memory to %d\n", critSec);
		SemPost(semGetID);
	}
}