#ifndef _SYSCALL_H_
#define _SYSCALL_H_

//Must include type.h for prototype
#include "type.h"

// no input, 1 return
int GetPid(void);

// no input, 1 return
int GetTime(void);

// 1 input, no return
void Sleep(int);


// Semaphore syscalls
int SemGet(void);
void SemPost(int semaphore_num);
void SemWait(int semaphore_num);
    
// Message Passing syscalls
void MsgSend(int mbox, msg_t *msg);
void MsgRecv(int mbox, msg_t *msg);

#endif