;/* 	2012-9  Created by dezhi jiang
; *	xxxxxx   please added here
; */

	EXTERN  raw_task_active
	EXTERN  high_ready_obj

	IMPORT  raw_stack_check


	EXPORT  OS_CPU_SR_Save    
	EXPORT  OS_CPU_SR_Restore

	EXPORT  CPU_IntDis
	EXPORT  CPU_IntEn



	EXPORT  raw_start_first_task

	EXPORT  port_task_switch
	EXPORT  raw_int_switch

	EXPORT  PendSV_Handler
    

;********************************************************************************************************
;                                                EQUATES
;********************************************************************************************************

NVIC_INT_CTRL   EQU     0xE000ED04                              ; Interrupt control state register.
NVIC_SYSPRI14   EQU     0xE000ED22                              ; System priority register (priority 14).
NVIC_PENDSV_PRI EQU           0xFF                              ; PendSV priority value (lowest).
NVIC_PENDSVSET  EQU     0x10000000                              ; Value to trigger PendSV exception.

;********************************************************************************************************
;                                      CODE GENERATION DIRECTIVES
;********************************************************************************************************

	AREA |.text|, CODE, READONLY, ALIGN=2
	THUMB
	REQUIRE8
	PRESERVE8

OS_CPU_SR_Save
	MRS     R0, PRIMASK     
	CPSID   I
	BX      LR

OS_CPU_SR_Restore
    MSR     PRIMASK, R0
    BX      LR


CPU_IntDis
	CPSID   I
	BX      LR

CPU_IntEn
	CPSIE   I
	BX      LR


raw_start_first_task
	LDR     R0, =NVIC_SYSPRI14                   
	LDR     R1, =NVIC_PENDSV_PRI
	STRB    R1, [R0]

	MOVS    R0, #0                                       
	MSR     PSP, R0

	;align msp to 8 byte
	MRS 	R0, MSP
	LSRS    R0, R0, #3
	LSLS    R0, R0, #3
	MSR     MSP, R0

	LDR     R0, =NVIC_INT_CTRL                                  
	LDR     R1, =NVIC_PENDSVSET
	STR     R1, [R0]

	CPSIE   I                                                  

OSStartHang
	B       OSStartHang                                         
	NOP

port_task_switch
	LDR     R0, =NVIC_INT_CTRL                             
	LDR     R1, =NVIC_PENDSVSET
	STR     R1, [R0]
	BX      LR

raw_int_switch
	LDR     R0, =NVIC_INT_CTRL                                  
	LDR     R1, =NVIC_PENDSVSET
	STR     R1, [R0]
	BX      LR


PendSV_Handler
	CPSID   I                                  
	MRS     R0, PSP                                  
	CBZ     R0, OS_CPU_PendSVHandler_nosave

	IF		{FPU} != "SoftVFP"
	VSTMFD	r0!, {d8 - d15} 		; push FPU register s16~s31
	ENDIF

	SUBS    R0, R0, #0x20                   
	STM     R0, {R4-R11}

	LDR     R1, =raw_task_active              
	LDR     R1, [R1]
	STR     R0, [R1]                      

	PUSH    {R14}
	bl      raw_stack_check
	POP     {R14}

OS_CPU_PendSVHandler_nosave
	LDR     R0, =raw_task_active         
	LDR     R1, =high_ready_obj
	LDR     R2, [R1]
	STR     R2, [R0]

	LDR     R0, =raw_task_active         
	LDR     R1, =high_ready_obj
	LDR     R2, [R1]
	STR     R2, [R0]

	LDR     R0, [R2]                         
	LDM     R0, {R4-R11}                  
	ADDS    R0, R0, #0x20

	IF		{FPU} != "SoftVFP"
	VLDMFD	r0!, {d8 - d15} 		; pop FPU register s16~s31
	ENDIF
		
	MSR     PSP, R0               
	ORR     LR, LR, #0x04                
	CPSIE   I
	BX      LR                     

	END
