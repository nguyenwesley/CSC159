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
		//Problem is somewhere here, only shows one digit
			for(i = strSize; i >= 0; i--)
			{
				*p = str[i] + VIDEO_MASK;
				p--;
			}
		}
	}
	return;
}


void numToString(int number, char stringAddr[]) {
	int a, b, c, d;
	d = number % 10;
	c = (number / 10) % 10;
	b = (number / 100) % 10;
	a = (number / 1000) % 10;
	stringAddr[4] = '\0';
	stringAddr[3] = d + 0x30;
	stringAddr[2] = c + 0x30;
	stringAddr[1] = b + 0x30;
	stringAddr[0] = a + 0x30;
	return;
}


int stringCounter(char myString[]) {
	int tempCount = 1;
	while(myString[tempCount] != '\0') {
		tempCount++;
	}
	return tempCount;
}
