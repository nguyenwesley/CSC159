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
		if (ch == 'g')
			breakpoint();
		
		/*
		*
		*	phase2
		*
		*/
		if (QisEmpty(kb.wait_q) != TRUE)
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
*	phase2, gonna need a lot of work
*
*/


void GetTimeService(tf_t *tf_p) {
	tf_p.eax = (sys_tick / 100);
	Loader(tf_p);
}


void WriteService(tf_t *tf_p) {
	/*
	*	the address of the str to print is given in 'eax' of the TF
	*	that tf_p points to
	*
	*	for each char in the string (that's not NUL) {
	*	call WriteChar with that char as the argument
	*	advance the string pointer
	*	}
	*
	*	call Loader with tf_p as the argument to (resume the same process)
	*/
	//THIS IS DEFINITELY WRONG, REVISE
	while ((*tf_p).eax != '\0') {
		WriteChar(tf_p.eax);
		tf_p.eax++;
	}
	Loader(tf_p);
}


void WriteChar(char c) {
	static unsigned short *cursor = (typecast)VIDEO_START;
	int curColPos;
	int remCol;
	
	//May need to adjust numbers
	if (((cursor - VIDEO_START) % 80) == 0) {
		while ((cursor % 79) != 0) {
			*cursor = 0x20 + VIDEO_MASK;
			cursor++;
		}
		cursor -= 0x4f;
	}
	
	if (c != CR && c != LF) {
		*cursor = c + VIDEO_MASK;
		cursor++;
	}
	else {
		curColPos = (cursor - VIDEO_START) % 80;
		remCol = 80 - curColPos;
		cursor += remCol;
	}
	
	if (cursor == (VIDEO_START + 0x78f))
		cursor = VIDEO_START;
}


void ReadService(tf_t *tf_p) {
	/*
	*	save tf_p to the PCB of the currently-running process
	*	
	*	move current PID to the wait queue of the keyboard
	*	alter its state to WAIT
	*	current PID becomes NA
	*	
	*	call Swapper to find another process to run
	*	call Loader to load the TF of the newly selected current PID to run it
	*/
	pcb[cur_pid].tf_p = tf_p;
	EnQ(cur_pid, kb.wait_q);
	pcb[cur_pid].state = WAIT;
	cur_pid = NA;
	Swapper();
	Loader(pcb[cur_pid].tf_p);
}


//Lower half of ReadService, called by TimerService()
void KbService(char c) {
	/*
	*	call WriteChar with the character to echo/display it
	*
	*	if the character is not CR {						// DOS uses CR
	*		call StrAdd to add it to the keyboard buffer	// save it
	*		return;
	*	}
	*
	*	call StrAdd to add a NUL to the keyboard buffer
	*	realease the process by dequeuing PID from the keyboard wait queue
	*	the string space is pointed to by 'eax' of the process TF
	*	call StrCpy to copy the keyboard.buffer to the process string space
	*	alter the released process state to READY
	*	move it to the ready queue
	*	clear/empty the keyboard buffer
	*/
}
