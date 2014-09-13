#include <raw_api.h>

#include <stm32f4xx.h>

#define USE_FPU   /* ARMCC */ (  (defined ( __CC_ARM ) && defined ( __TARGET_FPU_VFP )) \
                  /* IAR */   || (defined ( __ICCARM__ ) && defined ( __ARMVFP__ )) \
                  /* GNU */   || (defined ( __GNUC__ ) && defined ( __VFP_FP__ ) && !defined(__SOFTFP__)) )
                  
void raw_sys_tick_init(void)
{
	SysTick_Config(SystemCoreClock / RAW_TICKS_PER_SECOND);
}

RAW_VOID  *port_stack_init(PORT_STACK  *p_stk_base, RAW_U32 stk_size,  RAW_VOID   *p_arg, RAW_TASK_ENTRY p_task)
{
	PORT_STACK *stk;
	RAW_U32 temp = (RAW_U32)(p_stk_base + stk_size);

	temp &= 0xfffffff8;
	
	stk = (PORT_STACK  *)temp;

	
	#if USE_FPU
	
	*(--stk)  = (RAW_U32)0xaa;                  /* R? : argument                                      */ 
	*(--stk)  = (RAW_U32)0xa;                   /* FPSCR : argument                                      */
	*(--stk)  = (RAW_U32)0x15;                   /* S15 : argument                                               */        
	*(--stk)  = (RAW_U32)0x14;                   /* S14 : argument                                      */
	*(--stk)  = (RAW_U32)0x13;                   /* S13 : argument                                      */
	*(--stk)  = (RAW_U32)0x12;                   /* S12 : argument                                      */
	*(--stk)  = (RAW_U32)0x11;                   /* S11 : argument                                      */
	*(--stk)  = (RAW_U32)0x10;                   /* S10 : argument                                      */
	*(--stk)  = (RAW_U32)0x9;                   /* S9 : argument                                      */
	*(--stk)  = (RAW_U32)0x8;                   /* S8 : argument                                      */
	*(--stk)  = (RAW_U32)0x7;                   /* S7 : argument                                 */         
	*(--stk)  = (RAW_U32)0x6;                   /* S6 : argument                                      */        
	*(--stk)  = (RAW_U32)0x5;                   /* S5 : argument                                      */
	*(--stk)  = (RAW_U32)0x4;                   /* S4 : argument                                      */
	*(--stk)  = (RAW_U32)0x3;                   /* S3 : argument                                      */
	*(--stk)  = (RAW_U32)0x2;                   /* S2 : argument                                      */
	*(--stk)  = (RAW_U32)0x1;                   /* S1 : argument                                      */
	*(--stk)  = (RAW_U32)0x0;                   /* S0 : argument                                      */

	#endif
	                                       
	*(--stk)  =  (RAW_U32)0x01000000L;             /* xPSR                                               */
	*(--stk)  = (RAW_U32)p_task;                    /* Entry Point                                        */
	*(--stk)  = (RAW_U32)0xFFFFFFFEL;             /* R14 (LR) (init value will cause fault if ever used)*/
	*(--stk)  = (RAW_U32)0x12121212L;             /* R12                                                */
	*(--stk)  = (RAW_U32)0x03030303L;             /* R3                                                 */
	*(--stk)  = (RAW_U32)0x02020202L;             /* R2                                                 */
	*(--stk)  = (RAW_U32)0x01010101L;             /* R1                                                 */
	*(--stk)  = (RAW_U32)p_arg;                   /* R0 : argument                                      */


	#if USE_FPU
	
	/* FPU register s16 ~ s31 */
	*(--stk)  = (RAW_U32)0x31uL; 		   /* S31												  */
	*(--stk)  = (RAW_U32)0x30uL; 		   /* S30												  */
	*(--stk)  = (RAW_U32)0x29uL; 		   /* S29												  */
	*(--stk)  = (RAW_U32)0x28uL; 		   /* S28												  */
	*(--stk)  = (RAW_U32)0x27uL; 		   /* S27												  */
	*(--stk)  = (RAW_U32)0x26uL; 		   /* S26												  */
	*(--stk)  = (RAW_U32)0x25uL; 		   /* S25												  */
	*(--stk)  = (RAW_U32)0x24uL; 		   /* S24												  */
	*(--stk)  = (RAW_U32)0x23uL; 		   /* S23												  */
	*(--stk)  = (RAW_U32)0x22uL; 		   /* S22												  */
	*(--stk)  = (RAW_U32)0x21uL; 		   /* S21												  */
	*(--stk)  = (RAW_U32)0x20uL; 		   /* S20												  */
	*(--stk)  = (RAW_U32)0x19uL; 		   /* S19												  */
	*(--stk)  = (RAW_U32)0x18uL; 		   /* S18												  */
	*(--stk)  = (RAW_U32)0x17uL; 		   /* S17												  */
	*(--stk)  = (RAW_U32)0x16uL; 		   /* S16												  */

	#endif
	                                 		
	*(--stk)  = (RAW_U32)0x11111111L;             /* R11                                                */
	*(--stk)  = (RAW_U32)0x10101010L;             /* R10                                                */
	*(--stk)  = (RAW_U32)0x09090909L;             /* R9                                                 */
	*(--stk)  = (RAW_U32)0x08080808L;             /* R8                                                 */
	*(--stk)  = (RAW_U32)0x07070707L;             /* R7                                                 */
	*(--stk)  = (RAW_U32)0x06060606L;             /* R6                                                 */
	*(--stk)  = (RAW_U32)0x05050505L;             /* R5                                                 */
	*(--stk)  = (RAW_U32)0x04040404L;             /* R4                                                 */
	 return stk;
	 
}









