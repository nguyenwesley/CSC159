// proc.c, 159
// all user processes are coded here
// processes do not R/W kernel data or call kernel code, only via syscalls

//include spede.h and kernel.h
#include "spede.h"
#include "kernel.h"
//phase2, need to include syscall.h to call new OS services
#include "syscall.h"

//prototype any subroutines of Clock() here
void numToString(int number, char stringAddr[]);
int stringCounter(char myString[]);



void Clock(void) {
	int i;
	int strSize;
	char str[] = "    ";									// str for 4-digits, plus NUL
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
			}
		}
	}
	return;
}


//Adjusted in phase2 to utilize a loop
void numToString(int number, char stringAddr[]) {
	int temp, i, counter;
	int x = 0;
	for (i = 0; i < stringCounter(stringAddr); i++)
	{
		temp = 1000;
		for (counter = 0; counter < i; counter++)
			temp = temp / 10;
		stringAddr[i] = ((number / temp) % 10) + 0x30;
	}
	stringAddr[stringCounter(stringAddr)] = '\0';
	
	//Removes leading zeros
	while (stringAddr[x] == 0x30 && x != 3)
	{
		stringAddr[x] = 0x20;
		x++;
	}
	return;
}


int stringCounter(char myString[]) {
	int tempCount = 0;
	while(myString[tempCount] != '\0') {
		tempCount++;
	}
	return tempCount;
}


//phase2
void Init(void) {
	int curTime;
	char tPrompt[] = "The time is ";
	char qPrompt[] = "What do you say to a cup of coffee? ";
	char aPrompt[] = "The answer is ";
	char period[] = ".\n";
	char answer[STR_SIZE];
	char time_str[] = "    ";
	while (1) {
		curTime = get_time_call();
		numToString(curTime, time_str);
		
		//Doublecheck
		
		//Time Prompt
		write_call(tPrompt);
		write_call(time_str);
		write_call(period);
		
		//Question Prompt
		write_call(qPrompt);
		read_call(kb.buffer);
		
		//Answer Prompt
		write_call(aPrompt);
		write_call(answer);
		write_call(period);
	}
}


//phase3
void Shell(void) {
	int time, my_pid;
	
	/*
	*	declare a cmd str with STR_SIZE
	*
	*	infinite loop
	*		declare a pid string that can show two digits, and
	*		a time string that can show 4 digits
	*
	*		get time and convert it to a string
	*		get my PID and convert it to a string
	*
	*		write out the two strings, and the prompt (see demo)
	*	
	*		get input to a command string input
	*		write out which PID is getting what command (see demo)
	*
	*		if command string is 'exit'
	*			write out my PID and message exiting (see demo)
	*			call the exit service
	*
	*		if command string is NOT 'fork,' 'continue' to top
	*
	*		call ? = fork_call
	*		if the return is NA, write "fork failed!\n"
	*		else if the return is zero, write "I'm a new child process.\n"
	*		else write "I'm a parent process.\n"
	*	loop ends
	*/
}


/*
*	program StrCmp() given two char * (strings), and it returns TRUE
*	if they match, otherwise FALSE
*/
