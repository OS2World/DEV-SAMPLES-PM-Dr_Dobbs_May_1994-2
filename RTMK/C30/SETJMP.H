/****************************************************************************/
/*  setjmp.h v4.50                                                          */
/*  Copyright (c) 1992 Texas Instruments Inc.                               */
/****************************************************************************/
#ifndef _SETJMP
#define _SETJMP

typedef long jmp_buf[12];

#define setjmp(_x) _setjmp(_x)
void longjmp(jmp_buf _env, int _returnval);