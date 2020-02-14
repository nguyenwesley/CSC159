// boot.c, 159, phase 1
//
// Team Name: SOSOS (Members: David Williams, Wesley Nguyen, Yamini Dasu)

//needs to include spede.h kernel.h entry.h misc.h proc.h
#include "spede.h"
#include "kernel.h"
#include "entry.h"
#include "misc.h"
#include "proc.h"

// kernel data are all declared here (prototyped in kernel.h):
int cur_pid;
int x = 0;
q_t unused_q;
q_t ready_q;
pcb_t pcb[PROC_SIZE];
char stack[PROC_SIZE][STACK_SIZE];


unsigned sys_tick;
struct i386_gate *intr_table; 



void CreateProc(func_p_t *functionPT) {
	int pid;
	pid = DeQ(&unused_q);
	EnQ(pid, (q_t *)&ready_q);
	
	//clear both the PCB and stack for the new PID
	Bzero((char *)&pcb[cur_pid], sizeof(pcb_t));
	Bzero((char *)&stack[cur_pid][0], PROC_SIZE);
	
	//set its state to READY
	pcb[cur_pid].state = READY;
	
	//point its tf_p to the correct place in its stack
	pcb[cur_pid].tf_p = (tf_t *)&stack[cur_pid][STACK_SIZE - sizeof(tf_t)];
	//use tf_p to set its efl cs eip	
	(*pcb[cur_pid].tf_p).efl = FLAGS;
	(*pcb[cur_pid].tf_p).cs = CS;
	(*pcb[cur_pid].tf_p).eip = (unsigned) functionPT;
}


void main(void) {                   // kernel boots
	sys_tick = 0;
	intr_table = (struct i386_gate *)INTR_TABLE;

	//clear both unused_q and ready_q
	Bzero((char *)&unused_q, sizeof(q_t));
	Bzero((char *)&ready_q, sizeof(q_t));
	
	unused_q.head = unused_q.tail = unused_q.size = 0;
	ready_q.head = ready_q.tail = ready_q.size = 0;
	
	//add all unused PID's into unused_q
	while (x < Q_SIZE)
	{
		EnQ(x, (q_t *)&unused_q);
		x++;
	}
	
   	fill_gate(&intr_table[TIMER], (int)TimerEntry, get_cs(), ACC_INTR_GATE, 0);
	outportb(PIC_MASK_REG, PIC_MASK);

   	CreateProc((func_p_t *) Clock);
   	cur_pid = DeQ(&ready_q);
   	
	//call Loader to load the trapframe of the new process
   	Loader(pcb[cur_pid].tf_p);
}
