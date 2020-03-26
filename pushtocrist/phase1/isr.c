#include "spede.h"
#include "type.h"
#include "isr.h"
#include "tool.h"
#include "extern.h"
#include "proc.h"

void CheckSleep() {
    // For each process in the sleep queue, determine
    // if it needs to wake up
    // If yes, add it to the running process queue and set the state to READY
    // If not, add it back to the sleep queue
	int pid;
	int sleepSize = sleep_q.size;
	int i;
	for (i = 0; i < sleepSize; i++)
	{
		pid = dequeue(&sleep_q);
		if (pcb[pid].wake_time == system_time)
		{
			pcb[pid].state = READY;
			enqueue(pid, &run_q);
		}
		else
			enqueue(pid, &sleep_q);
	}
}

void NewProcISR() {
    int pid;

    // Check if we have any available (unused) processes
    // If not, "panic" and return
	if (unused_q.size == 0)
	{
		cons_printf("Panic! No available processes!\n");
		return;
	}
	
    // Dequeue the process from the unused queue
	pid = dequeue(&unused_q);
	
    // Set the process state to READY
    // Initialize other process control block variables
	pcb[pid].state = READY;
	pcb[pid].time = 0;
	pcb[pid].total_time = 0;
	pcb[pid].wake_time = 0;
	
    // Move the proces into the run queue
	enqueue(pid, &run_q);
	
    // Ensure the stack for the process is cleared
    bzero(stack[pid], sizeof(stack[pid]));

    // Allocate the trapframe data
    pcb[pid].trapframe_p = (trapframe_t *)&stack[pid][STACK_SIZE - sizeof(trapframe_t)];

    // If the PID is 0, this is our init process, otherwise it is a user process
    if (pid == 0) {
        pcb[pid].trapframe_p->eip = (unsigned int)InitProc;   // InitProc process
    } else {
        pcb[pid].trapframe_p->eip = (unsigned int)UserProc;   // other processes
    }

    // Set up the trapframe data
    pcb[pid].trapframe_p->eflags = EF_DEFAULT_VALUE|EF_INTR; // set INTR flag
    pcb[pid].trapframe_p->cs = get_cs();                     // standard fair
    pcb[pid].trapframe_p->ds = get_ds();                     // standard fair
    pcb[pid].trapframe_p->es = get_es();                     // standard fair
    pcb[pid].trapframe_p->fs = get_fs();                     // standard fair
    pcb[pid].trapframe_p->gs = get_gs();                     // standard fair
}

//Note: ExitISR is extremely finnicky, will consistently work when there
//		are more than 10 processes running
void ProcExitISR() {
    // If the running PID is 0 (InitProc) return so we don't ever exit InitProc
	if (run_pid == 0)
		return;
	
    // If the running PID is invalid, panic and set a breakpoint
	if (run_pid < 0 || run_pid > MAX_PROC - 1)
	{
		cons_printf("Panic! Invalid Running PID!\n");
		breakpoint();
	}
	
    // Display a message indicating that the process is exiting
	cons_printf("\tExiting Process #%d\n", run_pid);
	
    // Change the state of the running process to UNUSED
    // Queue it to the unused queue
    // clear the running pid
	pcb[run_pid].state = UNUSED;
	enqueue(run_pid, &unused_q);
}

void TimerISR() {
    // Increment the system time
	system_time++;
	
    // Check to see if we need to handle sleeping processes
	if (sleep_q.size > 0)
		CheckSleep();
	
    // If the running PID is invalid, just return
	if (run_pid < 0 || run_pid > MAX_PROC - 1)
		return;
	
    // Increment the running process' current run time
	pcb[run_pid].time++;
	
    // Once the running process has exceeded it's scheduled
    // time slice, we need to unschedule it:
    //   increase the total run time
    //   reset the current running time
    //   set the state to ready
    //   queue the process back into the running queue
    //   clear the running pid
	if (pcb[run_pid].time == T_SLICE)
	{
		pcb[run_pid].total_time += pcb[run_pid].time;
		pcb[run_pid].time = 0;
		pcb[run_pid].state = READY;
		enqueue(run_pid, &run_q);
		run_pid = -1;
	}
}

void GetPidISR() {
    // Don't do anything if the running PID is invalid
	if (run_pid < 0 || run_pid > MAX_PROC - 1)
		return;
	
    // Copy the running pid from the kernel to the
    // eax register via the running process' trapframe
	pcb[run_pid].trapframe_p->eax = run_pid;
}

void GetTimeISR() {
    // Don't do anything if the running PID is invalid
	if (run_pid < 0 || run_pid > MAX_PROC - 1)
		return;
	
    // Copy the system time from the kernel to the
    // eax register via the running process' trapframe
	pcb[run_pid].trapframe_p->eax = system_time;
}

void SleepISR() {
    // Calculate the wake time for the currently running process
	int sleep_time = pcb[run_pid].trapframe_p->eax * 100;
	pcb[run_pid].wake_time = system_time + sleep_time;
	
	// Add currently running process to the sleep queue
	enqueue(run_pid, &sleep_q);
	
	// Change the running process state to SLEEP
	pcb[run_pid].state = SLEEP;
	
	// Pull next ready process from the process queue
	run_pid = dequeue(&run_q);
}