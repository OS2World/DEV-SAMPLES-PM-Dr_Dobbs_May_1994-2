;
;  Intel Architecture specific routines.
;

    .MODEL small
    .CODE                       ; Create C compatible CS

; Offsets into the saved register area for each register
AX_OFST = 0
BX_OFST = 2
CX_OFST = 4
DX_OFST = 6
BP_OFST = 8
SI_OFST = 10
DI_OFST = 12
DS_OFST = 14
SS_OFST = 16
ES_OFST = 18
PSW_OFST= 20
PC_OFST = 22
SP_OFST = 24

INIT_PSW = 0200h        ;Thread's initial Processor Status Word


;====================================================================
; Return the current stack pointer.  This will be used as a
; reference for assigning the stack base for each thread.
;
; C Prototype:  char *InitStackBase( void );
;====================================================================
    PUBLIC	_InitStackBase
_InitStackBase	PROC
    mov ax, sp
    sub ax, 2       ;Where it will be after return
    ret
_InitStackBase ENDP

;====================================================================
;  asmContextSwitch - Switches processor context between two threads
;
;  C Prototype:
;           void asmContextSwitch( pregs *OldRegs, pregs *NewRegs );
;
;  NOTES:
;  1.  Assume SMALL or COMPACT memory model.  Don't save and
;      restore CODE, STACK, or DATA SEGMENTS.  These always
;      stay the same.
;  2.  Assume Microsoft and Borland C calling conventions
;      This routine will always be 'called' and the following
;      registers DO NOT need to be preserved across procedure
;      calls and are not saved and restored here:
;           AX, BX, CX, DX
;
;====================================================================
    PUBLIC	_asmContextSwitch
_asmContextSwitch	PROC

; Currently have:   SP   -> Return Address
;                   SP+2 -> Old reg save area pointer
;                   SP+4 -> New reg save area pointer
;
    push    si              ;Save old task's SI
    push    bp              ;And BP
    mov bp, sp              ;Get back to the base of the stack frame
    add bp, 4
    mov si, [bp+2]          ;Get pointer to old register save area
    pop [si+BP_OFST]        ;Save old process's BP in save area
    pop [si+SI_OFST]        ;and SI
    mov [si+DI_OFST], di    ;and rest of the regs that must be saved
    mov [si+ES_OFST], es
    pushf                   ;Push PSW onto the stack
    pop [si+PSW_OFST]       ;then pop into save area

; Save the return address as the saved PC and increment the SP before
; saving so context will be restored as if just returned to ReSched
    mov bx, [bp]            ;Get return address off the stack
    mov [si+PC_OFST], bx
    mov bx, sp              ;Increment SP
    add bx, 2
    mov [si+SP_OFST], bx    ;and save

    mov si, [bp+4]          ;Get new process's saved regs
    mov bp, [si+BP_OFST]    ;and restore registers
    mov di, [si+DI_OFST]
    mov es, [si+ES_OFST]
    push    [si+PSW_OFST]   ;Push new PSW onto the stack
    popf                    ;then pop into PSW
    mov sp, [si+SP_OFST]    ;Switch to new stack

; Push the saved PC on the stack to be restored when RET is executed
    push    [si+PC_OFST]
    mov si, [si+SI_OFST]    ;Finally, restore SI
    ret
_asmContextSwitch  ENDP

;====================================================================
; InitPregs
;   Sets the initial saved processor register for a new thread.
;
; C Prototype:  void InitPregs( pregs* pRegs,
;                               short  InitStack,
;                               short  MainRoutine );
;
;====================================================================
    PUBLIC	_InitPregs
_InitPregs PROC
    push    si
    push    bp
    mov bp, sp
    add bp, 4
    mov si, [bp+2]                  ;Get pointer to pregs

    ; Assume SMALL or COMPACT memory model and set the
    ; initial segments the same as the current ones
    mov [si+SS_OFST], ss
    mov [si+DS_OFST], ds
    mov [si+ES_OFST], es
    mov [si+PSW_OFST], INIT_PSW
    mov ax, [bp+4]
    mov [si+SP_OFST], ax        ;Stackbase
    mov ax, [bp+6]
    mov [si+PC_OFST], ax        ;Main Routine
    pop bp
    pop si
    ret
_InitPregs  ENDP

;====================================================================
; DisableInt - Disables Interrupts and returns the current
;              Processor Status Word
;
; C Prototype:  short DisableInt( void );
;====================================================================
    PUBLIC	_DisableInt
_DisableInt	PROC
    pushf
    pop     ax
    cli
    ret
_DisableInt ENDP

;====================================================================
; EnableInt - Enables interrupts IF enabled in the saved Processor
;             Status Word
;
; C Prototype:  void EnableInt( short );
;====================================================================
    PUBLIC	_EnableInt
_EnableInt	PROC
    push    bp
    mov     bp, sp
    mov     ax, [bp+4]      ;Get saved Processor Status Word
    and     ax, 0200h       ;If Interrupts were enabled
    jz      NoEnable
    sti                     ;then re-enable them
NoEnable:
    pop     bp
    ret
_EnableInt  ENDP

    END
