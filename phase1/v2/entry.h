// entry.h
// prototypes those in entry.S

#ifndef __ENTRY__
#define __ENTRY__

#ifndef ASSEMBLER  // skip below if ASSEMBLER defined (from an assembly code)
                   // since below is not in assembler syntax
__BEGIN_DECLS

#include "kernel.h"        // tf_t below needs it

/*
*
* Change this below
*
*/

void TimerEntry(void);     // coded in entry.S, assembler won't like this syntax
void Loader(tf_t *);       // coded in entry.S

__END_DECLS

#endif // ifndef ASSEMBLER

#endif // ifndef __ENTRY__

