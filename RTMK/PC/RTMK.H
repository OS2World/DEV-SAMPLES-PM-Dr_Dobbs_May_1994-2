/* functions' prototypes */

#include "rtmktype.h" /* declaration of system's types */

#ifndef RTMK_H
#define RTMK_H

void create_process(PROCESS *,void far *);

void send(PROCESS,SIGNALS);

SIGNALS wait(SIGNALS);

SIGNALS reset(SIGNALS);

SIGNALS arrived_signals(void);

SIGNALS process_state(PROCESS);

#define ANY_SIGNAL 0xffffffff

#define ALL_SIGNALS 0xffffffff

int run_kernel(void);

#endif /* RTMK_H */
