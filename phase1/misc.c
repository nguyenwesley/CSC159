// misc.c, 159

// include spede.h and kernel.h
#include "spede.h"
#include "kernel.h"



int QisEmpty(q_t *Queue) {
	if((*Queue).tail == 0)
		return TRUE;
	else
		return FALSE;
}


int QisFull(q_t *Queue) {
	if((*Queue).size ==  20)
		return TRUE;
	else
		return FALSE;
}


int DeQ(q_t *Queue) {
	int tempQHead = 0;
	if(QisEmpty(Queue))
		tempQHead = NA;
	else
	{
		tempQHead = (*Queue).q[0];
		(*Queue).head = (*Queue).q[1];
		(*Queue).size--;
	}
	return tempQHead;
}


void EnQ(int number, q_t *Queue) {
	if(QisFull(Queue)) {
		cons_printf("Panic: queue is full, cannot EnQ!\n");
		breakpoint();
	}
	else {
		(*Queue).q[(*Queue).size - 1] = number;
		(*Queue).tail++;
	}
}


void Bzero(char *memoryStart, int byteFillLength) {
	int tempFillLength = byteFillLength;
	while(tempFillLength >= 0) {
		*memoryStart= '\0';
		*memoryStart++;
		tempFillLength--;
	}
}
