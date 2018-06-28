//    threads.h
//      Multitasking class definitions
//
#ifndef THREADS_H
#define THREADS_H

#include "specific.h"

#define TRUE    1
#define FALSE   0

typedef void    (*vfptr)();

class Thread;
class Semaphore;

// Stack size values
#define MIN_STACK        0x400      // Minimum stack size per thread
#define NULL_STACK       0x400      // Space for NULL thread
#define INIT_STACK       0x080      // Space for scheduler initialization
#define DEFAULT_STACK    0x400      // Default size

// Values for thread states
#define THREAD_UNUSED   0           // Thread Table entry unused
#define THREAD_READY    1           // Thread is ready to run
#define THREAD_CURRENT  2           // Thread is currently running
#define THREAD_BLOCKED  3           // Blocked on a sem or timer

// Thread priorities
#define LOWEST_PRIORITY     4
#define HIGHEST_PRIORITY    0
#define NULL_PRIORITY       (LOWEST_PRIORITY+1)

// Max number of threads to allow
#define MAX_THREADS     8
#define MAX_THREADID    (MAX_THREADS-1)

//
//  Doubly Linked Queue class
//
class Dlque
{
private:
    Dlque	*flink;         // Forward Link
    Dlque	*blink;         // Backward Link
public:
    int Empty();                // Check for empty queue
    void Add( Dlque *Queue );   // Add to the back
    Dlque *Remove();            // Remove from the front
    void Delink();              // Remove from the middle
    Dlque *Peak();              // Look at front without removing.

    Dlque() { flink = blink = this; }
    ~Dlque() {}
};

//
//  The SCHEDULER class
//
class Scheduler
{
private:
    Thread  *ThreadTab[MAX_THREADS];
    Dlque   ReadyList[NULL_PRIORITY+1];
    char    *CurStackBase;
    Thread  *NullThread;
    void    ContextSwitch( pregs *OldRegs, pregs *NewRegs )
                { asmContextSwitch( OldRegs, NewRegs ); }

public:
     static Scheduler *InstancePtr;  // Ptr to one and only instance
    Thread  *CurrentThread;          // Current thread
    char    *GetStackSpace( unsigned Size );
    void    ReSched();               // Reschedule threads
    void    AddReady( Thread *pThread );
    void    RemoveReady( Thread *pThread );
    char    AddThread( Thread *pThread );
    void    Pause();
    void    StartMultiTasking();
    Scheduler();
    ~Scheduler() {}
};

// Scheduler initialization class.  Insures that only one instance
// of the Scheduler is created no matter how many modules include
// threads.h.  Also insures that it is created before any threads.
class SchedulerInit
{
private:
    static int count;       // Compile time initialized to 0

public:
    SchedulerInit() { if( count++ == 0 )
                        Scheduler::InstancePtr = new Scheduler; }
    ~SchedulerInit() { if( --count == 0 )
                          delete Scheduler::InstancePtr; }
};

static SchedulerInit   SchedInit;

//
// THREAD class
//
class Thread
{
private:
    friend class Scheduler;
    friend class Semaphore;
    Dlque       Queue;          // For putting threads on Queues
    pregs       Regs;           // Processor specific saved registers
    char        State;          // Current thread state
    static void ThreadRet();    // Called if a thread returns from main
public:
    char        id;             // Thread ID
    unsigned    Priority;
    void MakeReady() { State = THREAD_READY; }
    void MakeCurrent() { State = THREAD_CURRENT; }
    void MakeBlocked() { State = THREAD_BLOCKED; }
    Thread( vfptr MainRtn,
            unsigned Priority=LOWEST_PRIORITY,
            unsigned StackSpace=DEFAULT_STACK );
    ~Thread() {}
};

//
//  SEMAPHORE class
//
class Semaphore : Dlque
{
private:
    short       count;
public:
    void Wait();
    void Signal();
    Semaphore( short InitCount=0 );
    ~Semaphore() {}
};

#endif  // THREADS_H
