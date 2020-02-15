// misc.c, 159

// include spede.h and kernel.h
#include "spede.h"
#include "kernel.h"



int QisEmpty(q_t *Queue) {
	if((*Queue).size == NUL)
		return TRUE;
	else
		return FALSE;
}


int QisFull(q_t *Queue) {
	if((*Queue).size ==  Q_SIZE)
		return TRUE;
	else
		return FALSE;
}


//Incorrect, must recode
//edited, but not sure if correct
int DeQ(q_t *Queue) {
	int data = 0;
	if(QisEmpty(Queue))
		return NA;
	else
	{
		data = (*Queue).q[(*Queue).head];
		(*Queue).head++;
		(*Queue).size--;
	}
	return data;
}


//Incorrect, must recode
//edited, but not sure if correct
void EnQ(int number, q_t *Queue) {
	if(QisFull(Queue)) {
		cons_printf("Panic: queue is full, cannot EnQ!\n");
		breakpoint();
	}
	else {
		(*Queue).q[(*Queue).tail] = number;
		(*Queue).size++;
		(*Queue).tail++;
	}
}


void Bzero(char *memoryStart, int byteFillLength) {
	int tempFillLength = byteFillLength;
	while(tempFillLength > 0) {
		*memoryStart= '\0';
		memoryStart++;
		tempFillLength--;
	}
}
