#include        <stdio.h>
#include        <dos.h>
#include        "threads.h"

//====================================================================
//  Example Program
//    Uses two threads.  Thread1 is higher priority than thread2.
//  Installs a new ISR for the DOS keyboard interrupt which signals
//  a semaphore that is waited on by thread1.  Thread1 waits for three
//  interrupts and then signals a second semaphore which is waited on
//  by thread2.  Thread2 goes into a long loop each time it retuns
//  from Sem.Wait.  It does this a few times, restores the DOS ISR
//  and exits.
//====================================================================

// Allocate the two threads and semaphores 
void    ThreadOneMain(void);
void    ThreadTwoMain(void);

Semaphore       Sem1;
Semaphore       Sem2;

Thread  Thread1( ThreadOneMain, HIGHEST_PRIORITY ); 
Thread  Thread2( ThreadTwoMain, HIGHEST_PRIORITY+1 );

//  The keyboard interrupt vector
#define KBD_VECTOR 0x09

void (_cdecl _interrupt _far *SavedISR)();

// New keyboard ISR
void __cdecl __interrupt __far OurISR()
{
    // call the original DOS ISR so it can do its
    // processing including clearing the interrupt.
    SavedISR();

    Sem1.Signal();	// Signal the interrupt
}

void ThreadOneMain()
{
    int i;

    //
    // Collect a few interrupts then signal low priority thread.
    //
    while( TRUE ) {
        for( i=0; i<3; ++i ) {
            Sem1.Wait();
            printf( "Interrupt signalled\n" );
        }
        Sem2.Signal();
    }
}

void ThreadTwoMain()
{
    int i;
    unsigned long Count;

    // Save the original DOS ISR and install ours
    SavedISR = _dos_getvect( KBD_VECTOR );

    _dos_setvect( KBD_VECTOR, OurISR );
    
    for( i=0; i<2; ++i ) {
        Sem2.Wait();
        printf( "Low priority event received\n" );

        // Simulate long background processing
        for( Count=0; Count<0x000FFFFF; ++Count )
            ;
    }
    // That's enough.  Restore the original DOS ISR and return
    // from this thread's main routine which will end the program.

    _dos_setvect( KBD_VECTOR, SavedISR );
}
    
