/* RTMK.C Real Time Micro Kernel */

#include "RTMKTYPE.h"   /* RTMK types' definitions */
#include <dos.h>        /* include for context and interrupts management */

#define NULL 0
#define PROCESS_STACK_SIZE 500  /* Stack size for each process */

unsigned _stklen=20;    /* minimal stack needed to start the kernel */

/********************* System's variables ****************/

struct PCS pcs_tab[32]; /* Process Context Structure table */

unsigned stack[32*PROCESS_STACK_SIZE]; /* stack table for all the process */

unsigned nbr_process;   /* number of process declared */

PROCESS current_process;    /* pointer on current process PCS */

word32 ready_process;               /* bit map list of ready process */

/************************************************************************/
/*   create_process : declares the process where p is the identifier for*/
/* the kernel and entry_point is the address of the process's code      */
/* the context of the process is initialized.                           */
/************************************************************************/

void create_process(PROCESS *process_id,void far *entry_point())
{
  if (nbr_process<32){ /* 32 is the maximun number of process */
  *process_id=pcs_tab+nbr_process;
  (pcs_tab[nbr_process].context)->j_ip=FP_OFF(entry_point);
  (pcs_tab[nbr_process].context)->j_cs=FP_SEG(entry_point);
  (pcs_tab[nbr_process].context)->j_flag=0; /* reset flag register to disable interrupts */
	/* process stack */
  (pcs_tab[nbr_process].context)->j_sp=
			(unsigned)stack+PROCESS_STACK_SIZE*(32-nbr_process); 
  (pcs_tab[nbr_process].context)->j_bp=(pcs_tab[nbr_process].context)->j_sp;    /* bp=sp (stack) */
  (pcs_tab[nbr_process].context)->j_ds=FP_SEG((void far *)stack);
  (pcs_tab[nbr_process].context)->j_ss=FP_SEG((void far *)stack);
  nbr_process+=1;
  }
}

/************************************************************************/
/* scheduler:  the context  of the current process is saved and the     */
/*      system switch to the ready process.                             */
/*      if next_process=NULL the higher priority ready process is       */
/*      searched, else the process is the ready process.                */
/************************************************************************/

void scheduler(PROCESS next_process)
{word32 n,i;            /* i and n loop variables */

/* saves the context of current process */
  if (setjmp(current_process->context)==0){
    if (next_process)
    current_process=next_process;     /* the scheduled is the one in next_process */
    else {                            /* determine the next_process */
      n=0;
      i=0x80000000;
      while (!(i&ready_process)) {
	n+=1;
	i>>=1;
      }
      current_process=pcs_tab+n; /* the scheduled process is the elected process */
    }
  longjmp(current_process->context,1); /* switch to the scheduled process */
  }
}

/************************************************************************/
/*                      SIGNALS MANAGEMENT :                            */
/*                    send(process,signals_mask)                        */
/*                    wait(signals_mask)                                */
/*                    reset(signals_mask)                               */
/*                    arrived_signals()                                 */
/*                    process_state(process)                            */
/************************************************************************/

/************************************************************************/
/* send: send to process signals that are on (1) in signals_mask        */
/************************************************************************/

void send(process,signals_mask)
PROCESS process;
SIGNALS signals_mask;
{
  process->received_signals|=signals_mask;      /* update arrived signals */
  if (process->received_signals&process->expected_signals){
  	/* if the process is waiting for the signals */
    ready_process|=process->pmask;      /* put the process ready */
    process->expected_signals=0;        /* reset expected signals   */
    if (current_process->priority<process->priority){
    /* process's priority level is higher than current_process priority */
      scheduler(process); /* switch to process directly */
    }
  }
}

/************************************************************************/
/* wait: puts the current process in wait for signals_mask              */
/*                 return arrived signasl                               */
/************************************************************************/

SIGNALS wait(signals_mask)
SIGNALS signals_mask;
{
  if (!(current_process->received_signals&signals_mask)){
  /* if signals in signals_mask are not arrived */
	/* update expected_signals */
    current_process->expected_signals=signals_mask; 
    ready_process^=current_process->pmask; /* turn process not ready */
    scheduler(NULL);                    /* switch to next process */
  }
  return (current_process->received_signals); /* returns arrived signals */
}

/************************************************************************/
/* reset: puts signals that are on in signals_mask to zero(not arrived) */
/*             returns signals that were arrived                        */
/************************************************************************/

SIGNALS reset(signals_mask)
SIGNALS signals_mask;
{SIGNALS old_signals;
  old_signals=current_process->received_signals;  /* saves arrived signals */
					/* reset signals_mask */
  current_process->received_signals=
		current_process->received_signals&~signals_mask;
  return (old_signals);      /* returns arrived signals before reset */
}

/************************************************************************/
/* arrived_signals: returns arrived signals of the current process      */
/************************************************************************/

SIGNALS arrived_signals()
{
  return (current_process->received_signals); /* returns arrived signals */
}

/************************************************************************/
/* process_state: returns expected signals of process                   */
/************************************************************************/

SIGNALS process_state(process)
PROCESS process;
{
  return (process->expected_signals); /* returns expected signals */
}

/************************************************************************/
/* run_kernel: initialize the kernel's variables and switch to the      */
/*      first process, the last loop is the system idle process.        */
/************************************************************************/



word32 free_time;  /* time not used by process */

int run_kernel()
{int i;                         /* loop variable */
 word32 current_process_mask;   /* manage the process mask */
 PROCESS pcs_ptr;               /* pointer on process contect structure */
  disable();                    /* disable interrupts */

  /* initialization of process context structures */
  current_process_mask=0x80000000;
  ready_process=0;
  for(i=0;i<=nbr_process;i++){  /* for each process initialize pcs */
    pcs_ptr=pcs_tab+i;          /* point to the pcs in the pcs table */

    pcs_ptr->received_signals=0;                /* no events arrived */
    pcs_ptr->expected_signals=0;                /* no events expected */
    pcs_ptr->pmask=current_process_mask;        /* put the process mask */
    pcs_ptr->priority=nbr_process-i;            /* set the priority */

			/* the process is now ready to take the CPU */
    ready_process|=current_process_mask;  
		/* current_process_mask for the next process */
    current_process_mask=current_process_mask>>1;  
  }

  current_process=pcs_tab+nbr_process;  /* current process is idle process */
  free_time=0;          /* reset free_time */

  scheduler(pcs_tab);   /* switch to the higher priority process */

  enable();             /* enable interrupts in idle process */

  for(;;)       /* loop forever : idle process */

    free_time+=1;       /* one for each loop */
}
