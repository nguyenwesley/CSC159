// kernel.c, 159, phase 2
//
// Team Name: SOSOS (Members: David Williams, Wesley Nguyen, Yamini Dasu)

//include spede.h kernel.h proc.h misc.h entry.h
#include "spede.h"
#include "kernel.h"
#include "proc.h"
#include "misc.h"
#include "entry.h"



void TimerService(tf_t *tfProcess) {
	//save tf_p to the PCB of cur_pid
	pcb[cur_pid].tf_p = tfProcess;

	
	if (cons_kbhit()) {
		char ch = cons_getchar();
		//change 'g' to 'G' for phase3
		if (ch == 'G')
			breakpoint();
		
		/*
		*
		*	phase2
		*
		*/
		if (QisEmpty((q_t *)&kb.wait_q) != TRUE)
			KbService(ch);
	}

	outportb(PIC_CONTROL_REG, TIMER_ACK);
	sys_tick++;
	pcb[cur_pid].run_tick++;
	pcb[cur_pid].total_tick++;
	
	//Runtime management
	if(pcb[cur_pid].run_tick == TIME_SIZE) {
		EnQ(cur_pid, &ready_q);
		
		//This is the wrong state
		pcb[cur_pid].state = READY;
		cur_pid = NA;
		Swapper();
	}

   	//call Loader with the tf_p of cur_pid
	Loader(pcb[cur_pid].tf_p);
}


void Swapper() {
	if (QisEmpty(&ready_q)) {
		cons_printf("Kernel: panic, no more process ready to run!\n");
		breakpoint();
	}
	cur_pid = DeQ(&ready_q);
	pcb[cur_pid].run_tick = 0;
	pcb[cur_pid].state = RUN;
}


/*
*
*	phase2
*
*/
void GetTimeService(tf_t *tf_p) {
	(*tf_p).eax = (sys_tick / 100);
	Loader(tf_p);
}


void WriteService(tf_t *tf_p) {
	//Doublecheck
	while ((*tf_p).eax != '\0') {
		WriteChar((*tf_p).eax);
		(*tf_p).eax++;
	}
	Loader(tf_p);
}


void WriteChar(char c) {
	static unsigned short *cursor = (unsigned short *) VIDEO_START;
	int curColPos;
	int remCol;
	int counter = 0;
	
	//May need to adjust numbers
	if (((cursor - ((unsigned short *) VIDEO_START)) % 80) == 0) {
		while (counter != 79) {
			*cursor = 0x20 + VIDEO_MASK;
			cursor++;
			counter++;
		}
		counter = 0;
		cursor -= 0x4f;
	}
	
	if (c != CR && c != LF) {
		*cursor = c + VIDEO_MASK;
		cursor++;
	}
	else {
		curColPos = (cursor - ((unsigned short *) VIDEO_START)) % 80;
		remCol = 80 - curColPos;
		cursor += remCol;
	}
	
	if (cursor == (((unsigned short *) VIDEO_START) + 0x78f))
		cursor = ((unsigned short *) VIDEO_START);
}


void ReadService(tf_t *tf_p) {
	pcb[cur_pid].tf_p = tf_p;
	EnQ(cur_pid, (q_t *)&kb.wait_q);
	pcb[cur_pid].state = WAIT;
	cur_pid = NA;
	Swapper();
	Loader(pcb[cur_pid].tf_p);
}


//Lower half of ReadService, called by TimerService()
void KbService(char c) {
	int releasedPID;
	WriteChar(c);
	if (c != CR) {
		StrAdd(c, kb.buffer);
		return;
	}
	
	StrAdd(NUL, kb.buffer);
	releasedPID = DeQ((q_t *)&kb.wait_q);
	//the string space is pointed to by 'eax' of the process TF
	//call StrCpy to copy the keyboard.buffer to the process string space
	StrCpy(kb.buffer, (char *)(*pcb[releasedPID].tf_p).eax);
	
	pcb[releasedPID].state = READY;
	EnQ(releasedPID, (q_t *)&ready_q);
	Bzero(kb.buffer, STR_SIZE);
}


/*
*
*	phase3
*
*/
void GetPidService(tf_t *tf_p) {
	/*
	*	copy cur_pid into the eax of the trapframe that tf_p points to
	*	resume to run the same process
	*/
}


void ExitService(tf_t *tf_p) {
	/*
	*	reset the state of the running process, and
	*	reclaim the PID
	*	call Swapper to find another process to run
	*	call Loader to load its trapframe (to run)
	*/
}


void ForkService(tf_t *tf_p) {
	/*
	*	get a new Child pid from the unused PID queue
	*	if it's NA {
	*		copy NA to eax of trapframe (TF) using the given TF pointer
	*		resume to run the calling process
	*	}
	*
	*	Parent's eax in its TF = new Child PID
	*	add new Child pid to ready-to-run process queue
	*	
	*	Child gets a duplicate PCB from Parent
	*	set Child process state to READY
	*	set 'ppid' in Child's PCB to the calling process PID
	*	reset run_tick & total_tick of Child process
	*
	*	Child gets a duplicate of Parent's runtime stack
	*	calculate the stack distance between Child's and Parent's stacks
	*
	*	calculate Child's TF addr = TF ptr given to this function + stack distance
	*	add stack distance to ebp of Child's TF
	*
	*	in order to update all ebp in a nested subroutine calls (if occurred):
	*		use an integer pointer
	*		set it = the updated ebp (ebp is typecasted as an integer address)
	*		loop:
	*			if what the pointer points is not zero
	*			add stack distance to that non-zero value
	*			set the pointer = the updated value
	*			repeat the loop
	*
	*	Child's eax in its TF gets zero
	*
	*	resume to run the same process (Parent)
	*/
}
