#include "spede.h"      // spede stuff
#include "main.h"       // main stuff
#include "isr.h"        // ISR's
#include "tool.h"       // handy functions for Kernel
#include "proc.h"       // processes such as InitProc()
#include "type.h"       // processes such as InitProc()
#include "entry.h"      // in order to locate TimderEntry to set IDT
#include "syscall.h"    // For sytem calls

// kernel data stuff:
int run_pid;            // current running PID, if -1, no one running
int system_time;        // System time

// Process queues
q_t run_q;
q_t unused_q;
q_t sleep_q;


//Phase2 variables
mbox_t mbox_arr[MBOX_SIZE];
semaphore_t sem_arr[MAX_PROC];
q_t sem_q;

int semGetID = -1;
int critSec;

// Process table
pcb_t pcb[MAX_PROC];

// runtime stacks of processes
char stack[MAX_PROC][STACK_SIZE];

// Interrupt descriptor table
struct i386_gate *idt_p;

int main() {
    // Set the kernel data
    SetData();

    // Initialize the IDT
    SetControl();

    // Create InitProc
    NewProcISR();
	
	//Create 2 new system processes: PrinterProc and DispatcherProc
	NewProcISR();
	NewProcISR();

    // Run initial scheduler & Loader
    Scheduler();
    Loader(pcb[run_pid].trapframe_p);

    // never reached; but we must "return" a valid value
    return 0;
}

// Adds an etry to the IDT
void SetEntry(int entry_num, func_ptr_t func_ptr) {
    struct i386_gate *gateptr = &idt_p[entry_num];
    fill_gate(gateptr, (int)func_ptr, get_cs(), ACC_INTR_GATE, 0);
}

// Set initial kernel data
void SetData() {
	int i;
    // Ensure that we clear all of our queues using bzero from tool.c/.h
	bzero((char *)&run_q, Q_SIZE);
	bzero((char *)&unused_q, Q_SIZE);
	bzero((char *)&sleep_q, Q_SIZE);
	
	//Clearing queues for phase2
	bzero((char *)&sem_q, Q_SIZE);
	
    // Ensure that all processes are initially in our unused queue
    // State of processes should be UNUSED
	for (i = 0; i < Q_SIZE; i++)
	{
		enqueue(i, &unused_q);
		enqueue(i, &sem_q);
		pcb[i].state = UNUSED;
	}
	
    // Initiallize the running pid so the schedule will kick in
	run_pid = -1;
	
    // Initialize system time to 0
	system_time = 0;
}

void SetControl() {
    // To initialize the IDT we must get the IDT base
    idt_p = get_idt_base();

    // Add en entry for each interrupt into the IDT
	SetEntry(TIMER_INTR, TimerEntry);
	SetEntry(GETPID_INTR, GetPidEntry);
	SetEntry(GETTIME_INTR, GetTimeEntry);
	SetEntry(SLEEP_INTR, SleepEntry);

	//Adding entries for phase2
	SetEntry(SEMGET_INTR, SemGetEntry);
	SetEntry(SEMPOST_INTR, SemPostEntry);
	SetEntry(SEMWAIT_INTR, SemWaitEntry);
	SetEntry(MSGSEND_INTR, MsgSendEntry);
	SetEntry(MSGRECV_INTR, MsgRecvEntry);
	
    // Clear the PIC mask
    outportb(0x21, ~1);
}

void Scheduler() {
    // If we have an active process that is running, we
    // should simply return
	if (run_pid != -1)
		return;


	if (run_q.size != 0)
	{
		run_pid = dequeue(&run_q);
		pcb[run_pid].state = RUN;
	}
	else
	{
		cons_printf("Kernel Panic: no processes running!\n");
		breakpoint();
	}
}

void Kernel(trapframe_t *p) {
    char key;

    // save p into the PCB of the running process
    pcb[run_pid].trapframe_p = p;

    // Process the current interrupt
    //   Example: if it is the TIMER_INTR, call TimerISR();
    //   If an interrupt needs clear a flag, do so
    //   If we don't have an interrupt defined, panic and trigger a breakpoint
    switch (p->intr_num) {
        // Timer Interrupt
        case TIMER_INTR:
            // Call TimerISR
            TimerISR();

            // 0x20 is PIC control, 0x60 dismisses IRQ 0
            outportb(0x20, 0x60);
            break;

		case GETPID_INTR:
			//Call GetPIDISR
			GetPidISR();
			break;
		
		case GETTIME_INTR:
			//Call GetTimeISR
			GetTimeISR();
			break;
		
		case SLEEP_INTR:
			//Call SleepISR
			SleepISR();
			break;
		
		
		//Add INTRs for phase2
		case SEMGET_INTR:
			SemGetISR();
			break;
			
		case SEMPOST_INTR:
			SemPostISR();
			break;
			
		case SEMWAIT_INTR:
			SemWaitISR();
			break;
		
		case MSGSEND_INTR:
			MsgSendISR();
			break;
			
		case MSGRECV_INTR:
			MsgRecvISR();
			break;
			
        default:
            cons_printf("Kernel Panic: no such interrupt # (%d)!\n", p->intr_num);
            breakpoint();
    }

    // Process special developer/debug commands
    if (cons_kbhit()) {
        key = cons_getchar();

        switch(key) {
            case 'n':
                // Create a new process
                NewProcISR();
                break;

            case 'x':
                // Exit the next scheduled process
                ProcExitISR();
                break;

            case 'b':
                // Set a breakpoint
                breakpoint();
                break;

            case 'q':
                // Exit our kernel
                exit(0);
        }
    }

    Scheduler();
    Loader(pcb[run_pid].trapframe_p);
}