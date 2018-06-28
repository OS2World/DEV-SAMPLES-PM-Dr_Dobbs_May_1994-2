//  specific.h
//    Processor and compiler specific definitions
//
#ifndef SPECIFIC_H
#define SPECIFIC_H

// Intel processor saved register area.
struct pregs
{
    short    ax;     // Offset 0
    short    bx;     // 2
    short    cx;     // 4
    short    dx;     // 6
    short    bp;     // 8
    short    si;     // 10
    short    di;     // 12
    short    ds;     // 14
    short    ss;     // 16
    short    es;     // 18
    short    psw;    // 20
    short    pc;     // 22
    short    sp;     // 24
};

// Processor specific routines in specific.asm
extern "C" void asmContextSwitch( pregs*, pregs* );
extern "C" void InitPregs( pregs*, short, short );
extern "C" char *InitStackBase( void );
extern "C" short DisableInt( void );
extern "C" void EnableInt( short );

#endif
