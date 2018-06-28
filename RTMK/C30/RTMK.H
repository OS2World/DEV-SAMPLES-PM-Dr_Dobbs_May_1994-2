/* declaration des prototypes des primitives du noyau */

#include "rtmktype.h" /* declaration des types utilises par le noyau */

#ifndef RTMK_H
#define RTMK_H



void create_process(PROCESS *,void far (* entry_point) ());

void send(PROCESS,SIGNALS);

SIGNALS wait(SIGNALS);

SIGNALS reset(SIGNALS);

SIGNALS arrived_signals(void);

SIGNALS process_state(PROCESS);

#define ANY_SIGNAL 0xffffffff

#define ALL_SIGNALS 0xffffffff

int run_kernel(void);

#endif /* RTMK_H */
