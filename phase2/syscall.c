// syscall.c
// OS service calls

int get_time_call(void) {    // phase2
   int time;

   asm("int $48;            // jmp to inter table entry 48
        movl %%eax, %0"     // after intr, copy eax to time
       : "=g" (time)        // output line of asm(), one item
       :                    // input line of asm(), none
       : "eax"              // list of clobbered register(s)
   );

   return time;
}

void write_call(char *str) {
   asm("movl %0, %%eax;
        int $49"
       :
       : "g" ((int)str)
       : "eax"
   );
}

program read_call()...
