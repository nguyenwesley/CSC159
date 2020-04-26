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
	
	//If statement to catch an Invalid PID, prevents program from SEGFAULT when processes are killed
	//	and readded to run_q
	if (pid == -1)
		return;	
	
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


	/*
    // If the PID is 0, this is our init process, otherwise it is a user process
    if (pid == 0) {
        pcb[pid].trapframe_p->eip = (unsigned int)InitProc;   // InitProc process
    } else {
        pcb[pid].trapframe_p->eip = (unsigned int)UserProc;   // other processes
    }
	*/
	
	//Switch case used to handle process creation
	switch (pid)
	{
		case 0:
			pcb[pid].trapframe_p->eip = (unsigned int)InitProc;
			break;
			
		case 1:
			pcb[pid].trapframe_p->eip = (unsigned int)PrinterProc;
			break;
			
		case 2:
			pcb[pid].trapframe_p->eip = (unsigned int)DispatcherProc;
			break;
			
		default:
			pcb[pid].trapframe_p->eip = (unsigned int)UserProc;
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
	
	if (run_pid == -1)
		return;
	
    // If the running PID is invalid, panic and set a breakpoint
	if (run_pid > MAX_PROC - 1)
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
	run_pid = -1;
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
	if (pcb[run_pid].time >= T_SLICE)
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



//Semaphore ISRs
void SemGetISR() {
    /*
    *	Dequeue a semaphore from the semaphore queue
    *	If the semaphore is valid, ensure that the semaphore data is initialized
    *	Return the semaphore ID to the caller
    */
	int semID;
	
	semID = dequeue(&sem_q);
	
	if (semID > -1 && semID < MAX_PROC - 1)
	{
		bzero((char *)&sem_arr[semID], Q_SIZE);
	}
	pcb[run_pid].trapframe_p->eax = semID;
}

void SemPostISR() {
    /*
    *	If a process is in the semaphore's wait queue, enqueue it back to the running queue
    *	Decrement the semaphore access count (hint: what happens if the count < 0?)
    */
	int semID;
	int pid;
	
	semID = pcb[run_pid].trapframe_p->eax;
	if ((sem_arr[semID].wait_q).size > 0)
	{
		pid = dequeue(&sem_arr[semID].wait_q);
		enqueue(pid, &run_q);
		pcb[pid].state = RUN;
	}
	
	sem_arr[semID].count--;
}

void SemWaitISR() {
    /*
    *	Enqueue a process to the semaphore's wait queue if the semaphore is held
    *	Increment the semaphore access count
    */
	int semID;
	
	semID = pcb[run_pid].trapframe_p->eax;
	
	if (sem_arr[semID].count > 0)
	{
		enqueue(run_pid, &sem_arr[semID].wait_q);
		pcb[run_pid].state = WAIT;
		run_pid = -1;
	}
	
	sem_arr[semID].count++;
}

//Mailbox ISRs
void MsgSendISR() {
    /*
    *	Enqueue the message to the queue if no process is waiting
    *	If a process is waiting: 
    *	    a. Dequeue it, move it to the running queue 
    *	    b. Update the message pointer so the process in MsgRecvISR() can process it
    */
	msg_t *src, *dest;
	int mbox_num;
	int pid;
	int qSize;
	
	mbox_num = pcb[run_pid].trapframe_p->eax;
	qSize = (mbox_arr[mbox_num].wait_q).size;
	src = (msg_t *)pcb[run_pid].trapframe_p->ebx;
		
	if (qSize == 0)
	{
		msg_enqueue(src, &mbox_arr[mbox_num]);
	}
	else
	{
		pid = dequeue(&mbox_arr[mbox_num].wait_q);
		enqueue(pid, &run_q);
		pcb[pid].state = RUN;
		dest = (msg_t *)pcb[pid].trapframe_p->ebx;
		*dest = *src;
	}
}

void MsgRecvISR() {
    /*
    *	Dequeue a message from the message queue if one exists and return it to the user
	*	If there is no message in the queue, move the process to the wait queue
    */
	msg_t *src, *dest;
	int mbox_num;
	int qSize;
	
	mbox_num = pcb[run_pid].trapframe_p->eax;
	qSize = (mbox_arr[mbox_num].wait_q).size;
	dest = (msg_t *)pcb[run_pid].trapframe_p->ebx;
	if (qSize > 0)
	{
		src = msg_dequeue(&mbox_arr[mbox_num]);
		dest = (msg_t *)pcb[run_pid].trapframe_p->ebx;
		*dest = *src;
	}
	else
	{
		enqueue(run_pid, &mbox_arr[mbox_num].wait_q);
		pcb[run_pid].state = WAIT;
		run_pid = -1;
	}
}