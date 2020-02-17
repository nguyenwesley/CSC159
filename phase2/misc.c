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


//edited, but not sure if correct
int DeQ(q_t *Queue) {
	int data = 0;
	if(QisEmpty(Queue))
		return NA;
	else
	{
		data = (*Queue).q[(*Queue).head];
		(*Queue).head++;
		//if statement allows circular queue
		if ((*Queue).head >= Q_SIZE)
			(*Queue).head = 0;
		(*Queue).size--;
	}
	return data;
}


//edited, but not sure if correct
void EnQ(int number, q_t *Queue) {
	if(QisFull(Queue)) {
		cons_printf("Panic: queue is full, cannot EnQ!\n");
		breakpoint();
	}
	else {
		(*Queue).q[(*Queue).tail] = number;
		//if statement allows circular queue
		if ((*Queue).head >= Q_SIZE)
			(*Queue).head = 0;
		(*Queue).tail++;
		(*Queue).size++;
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


//phase2 function
void StrAdd(char toAdd, char *string[]) {
	//find end of the string by advancing the pointer
	while (*string != NUL)
		string++;
	
	/*
	*
	*	DOUBLECHECK CODE HERE
	*
	*/
	//Append new character
	*string = toAdd;
	//Append NUL to keep strin NUL-delimited
	string++;
	*string = NUL;
}


/*
*
*	DOUBLECHECK CODE HERE
*
*/
//phase2 function
void StrCpy(char *src, char *dest) {
	//Make copy
	*src = *dest;
	
	//Make sure dest copy is NUL-delimited
	dest++;
	if (dest != NUL)
		*dest = NUL;
}
