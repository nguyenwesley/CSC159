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
*	phase2
*
*/
void GetTimeService(tf_t *tf_p) {
	tf_p.eax = (sys_tick / 100);
	Loader(tf_p);
}


void WriteService(tf_t *tf_p) {
	//Doublecheck
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
	pcb[cur_pid].tf_p = tf_p;
	EnQ(cur_pid, kb.wait_q);
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
	releasedPID = DeQ(kb.wait_q);
	//the string space is pointed to by 'eax' of the process TF
	//call StrCpy to copy the keyboard.buffer to the process string space
	pcb[releasedPID].state = READY;
	EnQ(releasedPID, ready_q);
	Bzero(kb.buffer, STR_SIZE);
}
