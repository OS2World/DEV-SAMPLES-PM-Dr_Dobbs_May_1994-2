	.length  78
	.width   132
	.include "safesp.h"
********************************************************
* setjmp longjmp v4.50
* Copyright (c) 1992 Texas Instruments Incorporated
********************************************************
*    C syntax:       int setjmp(jmp_buf env)
*
*    Description:    Save caller's current environment for a subsequent
*                    call to longjmp.  Return 0.
*    Return:         Always returns 0.                    
*********************************************************
        .global  _setjmp, __setjmp, _longjmp

****************************************************************************
*  NOTE : ANSI specifies that "setjmp.h" declare "setjmp" as a macro. 
*         In our implementation, the setjmp macro calls a function "_setjmp".
*         However, since the user may not include "setjmp.h", we provide
*         two entry-points to this function.
****************************************************************************
__setjmp:
_setjmp:
	POP       R1                      ;get return address
        SAFESP    "LDI SP,AR0"
	.if .REGPARM == 0
        LDI       *AR0,AR2                ;get the location of jmp_buf arg.
	.endif

****************************************************************************
* Save all registers guaranteed to be preserved across calls in the jmp_buf.
****************************************************************************
        STI       R4,*AR2++               ;save register variables
        STI       R5,*AR2++               ;  (R4 - AR7)
        STF       R6,*AR2++
        STF       R7,*AR2++
	.if .tms320C40
	STI       R8,*AR2++
	.endif
        STI       AR3,*AR2++              ;save caller's FP
        STI       AR4,*AR2++
        STI       AR5,*AR2++
        STI       AR6,*AR2++
        STI       AR7,*AR2++
	BD        R1                      ;delayed branch to return
        STI       R1,*AR2++               ;save return address
        STI       AR0,*AR2                ;save caller's SP
        LDI       0,R0                    ;setjmp returns zero
****    B         R1                      ;BRANCH OCCURS

        .page
****************************************************************************
*    C syntax:    void longjmp(jmp_buf env, int returnvalue)
*
*    Description: Restore the context contained in the jump buffer.
*                 This causes an apparent "2nd return" from the
*                 setjmp invocation which built the "env" buffer.
*
*    Return:      This return appears to return "returnvalue", which must 
*                 be non-zero.
*
****************************************************************************
_longjmp:
	.if .REGPARM == 0
        SAFESP    "LDI SP,AR0"
        LDI       *-AR0(1),AR2        ;get location of jmp_buf
        LDI       *-AR0(2),R0         ;get return value
        LDIZ      1,R0                ;ensure non-zero return value
	.else
        LDI       R2,R0               ;return value
        LDIZ      1,R0                ;ensure non-zero return value
	.endif
 
****************************************************************************
* Restore environment from saved jmp_buf.
****************************************************************************
        LDI       *AR2++,R4           ;restore register variables
        LDI       *AR2++,R5           ;   (R4 - AR7)
        LDF       *AR2++,R6 
        LDF       *AR2++,R7 
	.if .tms320C40
	LDI       *AR2++,R8
	.endif
        LDI       *AR2++,AR3          ;restore FP
        LDI       *AR2++,AR4 
        LDI       *AR2++,AR5 
        LDI       *AR2++,AR6 
        LDI       *AR2++,AR7 
        LDI       *AR2++,R1           ;get return address
        SAFESP    "LDI *AR2,SP"       ;restore SP
    
        B         R1                  ;branch to setjmp for return
        .end
