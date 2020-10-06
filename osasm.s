

        AREA |.text|, CODE, READONLY, ALIGN=2
        THUMB
        REQUIRE8
        PRESERVE8
        
		EXTERN  OSRunningThreadPtr
		EXPORT  OS_ASM_Disable_Interrupts 
		EXPORT  OS_ASM_Enable_Interrupts
        EXPORT  SysTick_Handler
		EXPORT  OS_Start	
			
			
OS_ASM_Disable_Interrupts 
                CPSID I   ; Disable interrupts and configurable fault handlers 
                BX    LR  ; Is branch indirect (register).

OS_ASM_Enable_Interrupts 
                CPSIE I   ; Enable interrupts and configurable fault handlers 
                BX    LR  ; Is branch indirect (register).

SysTick_Handler                ; 1) Saves R0-R3,R12,LR,PC,PSR
    CPSID   I                  ; 2) Prevent interrupt during switch
    PUSH    {R4-R11}           ; 3) Save remaining regs r4-11
    LDR     R0, =OSRunningThreadPtr         ; 4) R0=pointer to RunPt, old thread
    LDR     R1, [R0]           ;    R1 = RunPt
    STR     SP, [R1]           ; 5) Save SP into TCB
    LDR     R1, [R1,#4]        ; 6) R1 = RunPt->next
    STR     R1, [R0]           ;    RunPt = R1
    LDR     SP, [R1]           ; 7) new thread SP; SP = RunPt->sp;
    POP     {R4-R11}           ; 8) restore regs r4-11
    CPSIE   I                  ; 9) tasks run with interrupts enabled
    BX      LR                 ; 10) restore R0-R3,R12,LR,PC,PSR


OS_Start
    LDR     R0, =OSRunningThreadPtr         ; currently running thread
    LDR     R2, [R0]           ; R2 = value of RunPt
    LDR     SP, [R2]           ; new thread SP; SP = RunPt->stackPointer;
    POP     {R4-R11}           ; restore regs r4-11
    POP     {R0-R3}            ; restore regs r0-3
    POP     {R12}
    POP     {LR}               ; discard LR from initial stack
    POP     {LR}               ; start location
    POP     {R1}               ; discard PSR
    CPSIE   I                  ; Enable interrupts at processor level
    BX      LR                 ; start first thread

    ALIGN
    END
