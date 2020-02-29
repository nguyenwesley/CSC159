// misc.h, 159

#ifndef __MISC__
#define __MIsC__

#include "kernel.h"						// need definition of 'q_t' below


int QisEmpty(q_t *);
int QisFull(q_t *);
int DeQ(q_t *);
void EnQ(int number, q_t *);
void Bzero(char *, int byteFillLength);

//phase2 prototypes
void StrAdd(char toAdd, char *string);
void StrCpy(char *src, char *dest);

//phase3 prototypes
void MemCpy(char *s, char *t, unsigned bytes);
#endif
