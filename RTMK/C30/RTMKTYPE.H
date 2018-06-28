/* Definition of system's types  */

#ifndef RTMK_TYPE_H
#define RTMK_TYPE_H

#include "setjmp.h"     /* for management of processor context */

typedef unsigned long word32;   /* Double Word for 32 process max kernel */

typedef word32 SIGNALS;       /* Signal type */

struct PCS {
		jmp_buf context;        /* CPU Context (registers) */
		SIGNALS expected_signals;
		SIGNALS received_signals;
		word32 pmask;
		word32 priority;
		};

typedef struct PCS *PROCESS;       /* pointer of PROCESS CONTEXT STRUCTURE */

#define enable() asm(" OR 2000h,ST");
#define disable() asm(" ANDN 2000h,ST"); 

#endif /* RTMK_TYPE_H */
