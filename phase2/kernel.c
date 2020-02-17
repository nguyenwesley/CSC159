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


void GetTimeService(tft_t *tf_p) {
	/*
	*	copy current second # to 'eax' in the TF that tf_p points to
	*	call Loader with tf_p as the argument to (resume the same process
	*/
}


void WriteService(tft_t *tf_p) {
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
}


void WriteChar(char c) {
	static unsigned short *cursor = (typecast)VIDEO_START;
	/*
	*	if 'cursor' is at the beginning of a row, erase that row (with spaces)
	*	
	*	if ch is neither CR nor LF {							// it's a regular character
	*		apply pointer 'cursor' to display the character (arg passed)
	*		advance 'cursor'
	*	} else {
	*		advance cursor to 1st column of next row:
	*			current column position = (cursor - VIDEO_START) % 80
	*			remaining columns in this row = 80 - current column position
	*			add remaining columns to cursor
	*	}
	*
	*	if 'cursor' reaches bottom-right on screen, set it back to top-left
	*/
}


void ReadService(tft_t *tf_p) {
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
