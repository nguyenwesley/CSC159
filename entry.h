#ifndef _ENTRY_H_
#define _ENTRY_H_

#include <spede/machine/pic.h>

// Interrupt definitions
#define TIMER_INTR    32
#define GETPID_INTR   48
#define GETTIME_INTR  49
#define SLEEP_INTR    50

//New system calls; allow user-space processes to interact with kernel. 
#define SEMGET_INTR   51
#define SEMPOST_INTR  52
#define SEMWAIT_INTR  53
#define MSGSEND_INTR  54
#define MSGRECV_INTR  55

#define KCODE 0x08         // kernel's code segment
#define KDATA 0x10         // kernel's data segment
#define KSTACK_SIZE 16384  // kernel's stack byte size

// ISR Entries
#ifndef ASSEMBLER

__BEGIN_DECLS

extern void TimerEntry();     // code defined in entry.S
extern void GetPidEntry();    // code defined in entry.S
extern void GetTimeEntry();   // code defined in entry.S
extern void SleepEntry();     // code defined in entry.S
extern void Loader();         // code defined in entry.S

//Added for phase 2
extern void SemGetEntry();
extern void SemPostEntry();
extern void SemWaitEntry();
extern void MsgSendEntry();
extern void MsgRecvEntry();

__END_DECLS

#endif
#endif