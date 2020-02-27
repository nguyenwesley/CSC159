// entry.S, entries to kernel, build process trapframe

#include <spede/machine/asmacros.h>				// do not use/include "spede.h"

#define KERNEL_STACK_SIZE 32768					// 32 KB should be enough

.text
.comm kernel_stack, KERNEL_STACK_SIZE, 1		// ... by unit of 1 byte

// loads the trapframe of the process to run it
ENTRY(Loader)
	movl 4(%esp), %ebx							// copy what is above stack pointer
	movl %ebx, %esp								// copy to esp (point esp to trapframe)
	popal										// pop edi esi ebp esp ebx edx ecx eax
	iret										// pop eip, cs, efl

// build trapframe then go to kernel service code
ENTRY(TimerEntry)								// push efl, cs, eip by hardware
	pushal										// push eax ecx edx ebx esp ebp esi edi
	movl %esp, %ecx
	leal kernel_stack+KERNEL_STACK_SIZE, %esp	// to kernel stack
	pushl %ecx									// push (proc trapframe addr) there
	cld											// clear direction flag bit
	call CNAME(TimerService)					// call C function code



/*
*
*	phase2
*
*/
ENTRY(GetTimeEntry)
	pushal
	movl %esp, %ecx
	leal kernel_stack+KERNEL_STACK_SIZE, %esp
	pushl %ecx
	cld
	call CNAME(GetTimeEntry)


ENTRY(WriteEntry)
	pushal
	movl %esp, %ecx
	leal kernel_stack+KERNEL_STACK_SIZE, %esp
	pushl %ecx
	cld
	call CNAME(WriteEntry)


ENTRY(ReadEntry)
	pushal
	movl %esp, %ecx
	leal kernel_stack+KERNEL_STACK_SIZE, %esp
	pushl %ecx
	cld
	call CNAME(ReadEntry)
