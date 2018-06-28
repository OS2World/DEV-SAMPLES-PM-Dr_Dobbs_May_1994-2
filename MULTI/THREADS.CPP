//
//  threads.cpp
//      Implementation of Multitasking Classes.
//
#include        <stdio.h>
#include        <stdlib.h>
#include        "threads.h"

#define	TRUE	1
#define FALSE	0

// Static count of SchedulerInit object that have been created.
int SchedulerInit::count = 0;

// Pointer to the one instance of the scheduler.
Scheduler *Scheduler::InstancePtr;

//===================================================================
//  Dlque::Empty
//      Returns TRUE if the Dlque is empty.
//===================================================================
inline int Dlque::Empty()
{
    return( flink == this );
}

//===================================================================
//  Dlque::Add
//      Adds an item to the end of a doubly linked queue.
//===================================================================
void Dlque::Add( Dlque *Queue )
{
    Queue->blink = blink;
    Queue->flink = this;
    blink->flink = Queue;
    blink = Queue;
}

//===================================================================
//  Dlque::Remove
//      Removes the item at the head of the dlque.  NULL if empty.
//===================================================================
Dlque *Dlque::Remove()
{
    Dlque   *Item;

    if( Empty() )  {
        return( NULL );
    }
    Item = flink;
    flink = Item->flink;
    Item->flink->blink = Item->blink;
    Item->flink = Item->blink = Item;

    return( Item );
}

//===================================================================
//  Dlque::Delink
//      Delinks an item from the middle of a dlque.
//===================================================================
void Dlque::Delink()
{
    blink->flink = flink;
    flink->blink = blink;
    flink = blink = this;
}

//===================================================================
//  Dlque::Peak
//      Returns a pointer to the first item without removing it.
//===================================================================
Dlque *Dlque::Peak()
{
    if( Empty() ) {
        return( NULL );
    }
    return( flink );
}


//===================================================================
//  Scheduler Constructor
//===================================================================
Scheduler::Scheduler()
{
    short       i;

    InstancePtr = this;

    // Initialize the Thread Table
    for( i=0; i<MAX_THREADS; ++i ) {
        ThreadTab[i] = NULL;
    }

    // Initialize System Stack Base to the current stack pointer
    CurStackBase = InitStackBase();

    // Allocate space for scheduler initialization
    CurStackBase -= INIT_STACK;

    // Create the NULL Thread.
    NullThread = new Thread( NULL, NULL_PRIORITY, NULL_STACK );
}

//===================================================================
//  GetStackSpace - Used by new threads to get their initial SP
//===================================================================
char *Scheduler::GetStackSpace( unsigned Size )
{
    char *Base;

    if ( Size < MIN_STACK ) {
        Size = MIN_STACK;
    }
    Base = CurStackBase;
    CurStackBase -= Size;   // Assume stack grows toward low mem.
    return Base;
}

//===================================================================
//  Scheduler::AddThread
//      Add a new thread into the Scheduler
//===================================================================
char Scheduler::AddThread( Thread *pThread )
{
    register char id;

    for( id=0; id<MAX_THREADS; ++id ) {
        if( ThreadTab[id] == NULL ) {
            break;
        }
    }
    if( id == MAX_THREADS ) {
        return( FALSE );
    }

    ThreadTab[id] = pThread;
    pThread->MakeReady();   // Tell new thread to make itself READY
    AddReady( pThread );    // Add to ready list in the scheduler
    return( TRUE );
}

//===================================================================
//  AddReady - Add the given thread to the ReadyList
//===================================================================
inline void Scheduler::AddReady( Thread *pThread )
{
    ReadyList[pThread->Priority].Add( &pThread->Queue );
}

//===================================================================
//  RemoveReady - Remove the specified thread from the ready list.
//===================================================================
inline void Scheduler::RemoveReady( Thread *pThread )
{
    pThread->Queue.Delink();
}

//===================================================================
//  Scheduler::ReSched
//      Picks the next ready thread and calls ContextSwitch to
//      perform the context switch to the new thread.
//===================================================================
void Scheduler::ReSched()
{
    Thread              *OldThread;
    Thread              *NewThread;
    unsigned            Priority;

    for( Priority=0; Priority<=NULL_PRIORITY; ++Priority ) {
        if( !ReadyList[Priority].Empty() ) {
            NewThread = (Thread *)ReadyList[Priority].Remove();
            break;
        }
    }
 
    // If calling thread is still ready and is the highest
    // priority ready thread, just return
    if( NewThread == CurrentThread ) {
        CurrentThread->MakeCurrent();
        return;
    }

    OldThread = CurrentThread;
    CurrentThread = NewThread;
    CurrentThread->MakeCurrent();
    ContextSwitch( &OldThread->Regs, &CurrentThread->Regs );
}

//===================================================================
//  Scheduler::Pause
//      Checks for any ready threads that are equal or higher
//      priority than the calling thread.  If so, reshcedules.
//===================================================================
void Scheduler::Pause()
{ 
    short       SavedPS;

    SavedPS = DisableInt();
    CurrentThread->MakeReady(); // Switch from Current to Ready
    AddReady( CurrentThread );  // Caller back on end of ReadyList
    ReSched();                  // Run new highest priority thread
    EnableInt( SavedPS );
}

//==================================================================
//  StartMultiTasking
//     Perform the transformation from a single threaded application
// to a set of threads running in their own individual contexts.
//     This is done by first setting up the system variables to look
// like the Null thread is the current thread.  Then, call Pause()
// which will cause the context of this routine to be saved as
// the NULL thread's context.  When the Null thread is rescheduled,
// the CPU will return to this routine.  The rest of this routine
// then becomes the loop that runs in the Null thread.
//===================================================================
void Scheduler::StartMultiTasking()
{
    RemoveReady( NullThread );
    CurrentThread = NullThread;
    NullThread->MakeCurrent();
    Pause();

    while( TRUE );      // Loop in the NULL thread
}
    
//===================================================================
//  Thread::Thread
//      Creates a new thread based on the specified params.
//===================================================================
Thread::Thread( vfptr MainRtn,
                unsigned TaskPriority,
                unsigned StackSpace )
:Queue()
{
    short       *StackPtr;
    short       RegContents;

    // Set up the initial stack so that if the main routine for this
    // thread returns for some reason, it returns to ThreadRet
    StackPtr
        = (short*)Scheduler::InstancePtr->GetStackSpace(StackSpace);
    *StackPtr = (short)Thread::ThreadRet;

    // Call processor/compiler specific routine to initialize
    // the saved processor registers.
    InitPregs( &this->Regs, (short)StackPtr, (short)MainRtn );

    Priority = TaskPriority;
    Scheduler::InstancePtr->AddThread( this );
    MakeReady();                    // Set our state to READY
}

//===================================================================
//  ThreadRet
//      Routine that is placed on each thread's stack as the return
//      address in case the 'main' routine ever returns.
//===================================================================
void Thread::ThreadRet()
{
#ifdef _DEBUG
    printf( "A Thread returned from main()\n" );
#endif
    exit( 1 );
}

//===================================================================
//  Semaphore::Semaphaore
//      Constructor for objects of the class semaphore.
//===================================================================
Semaphore::Semaphore( short InitCount )
{
    count = InitCount;
}

//===================================================================
//  Semaphore::Wait

//      Queue a thread as a waiter on a semaphore
//===================================================================
void Semaphore::Wait()
{
    short       SavedPS;

    SavedPS = DisableInt();
    if( count )         // No need to block waiter
    {
        --count;
    }
    else                // Waiter must block
    {
        Scheduler::InstancePtr->CurrentThread->MakeBlocked();
        Add( &Scheduler::InstancePtr->CurrentThread->Queue );
        Scheduler::InstancePtr->ReSched();
    }
    EnableInt( SavedPS );
}

//===================================================================
//  Semaphore::Signal
//      Signal a semaphore
//===================================================================
void Semaphore::Signal()
{
    short       SavedPS;
    Thread      *Waiter;

    SavedPS = DisableInt();
    if( Empty() )                       // No waiters to reschedule
    {
        ++count;
    }
    else                                // There are blocked waiters
    {
        Waiter = (Thread*)Remove();     // Get next waiter
        Waiter->MakeReady();            // Make it ready
        Scheduler::InstancePtr->AddReady( Waiter );
        if( Waiter->Priority <
          Scheduler::InstancePtr->CurrentThread->Priority ) {
            Scheduler::InstancePtr->CurrentThread->MakeReady();
            Scheduler::InstancePtr->AddReady(
                            Scheduler::InstancePtr->CurrentThread );
            Scheduler::InstancePtr->ReSched();
        }
    }
    EnableInt( SavedPS );
}

//===================================================================
//  main()
//===================================================================
void main()
{
    Scheduler::InstancePtr->StartMultiTasking();
}
