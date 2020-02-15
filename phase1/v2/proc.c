// proc.c, 159
// all user processes are coded here
// processes do not R/W kernel data or call kernel code, only via syscalls

//include spede.h and kernel.h
#include "spede.h"
#include "kernel.h"

//prototype any subroutines of Clock() here
void numToString(int number, char stringAddr[]);
int stringCounter(char myString[]);



void Clock(void) {
	int i;
	int strSize;
	char str[] = "    ";  // str for 4-digits, plus NUL
	unsigned short *p;
	strSize = stringCounter(str);
	while(1) {
		if(sys_tick % 100 == 0) {
			numToString(sys_tick/100, str);
			p = (unsigned short*) VIDEO_START;
			p = p + 0x4b;
			
			//need to fix
			for(i = 0; i < strSize; i++)
			{
				*p = str[i] + VIDEO_MASK;
				p++;
			}		//Displays all four digits, will fix in later versions
		}
	}
	return;
}


//Adjusted in phase2 to utilize a loop
void numToString(int number, char stringAddr[]) {
	int temp, i, counter;
	for (i = 0; i < stringCounter(stringAddr); i++)
	{
		temp = 1000;
		for (counter = 0; counter < i; counter++)
			temp = temp / 10;
		stringAddr[i] = ((number / temp) % 10) + 0x30;
	}
	stringAddr[stringCounter(stringAddr)] = '\0';
	return;
}


int stringCounter(char myString[]) {
	int tempCount = 0;
	while(myString[tempCount] != '\0') {
		tempCount++;
	}
	return tempCount;
}
