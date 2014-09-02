#include <stm32f4xx.h>

#include <raw_api.h>


/******************************************************************************/
#define DEBUG_SHELL_TASK_STK_SIZE 	512
static  PORT_STACK 				debug_shell_task_stk[DEBUG_SHELL_TASK_STK_SIZE];
static  RAW_TASK_OBJ 			debug_shell_task_obj;
/******************************************************************************/


int debug_write(const void *buf, int len)
{
	return 0;
}



/******************************************************************************/

RAW_TASK_OBJ *get_shell_task_obj(void)
{
	return &debug_shell_task_obj;
}

static void debug_shell_task(void *pdat)
{	
	(void)pdat;
//	uart0_init(115200);
//	if(debug_fifo_init() < 0)
//	{
//		for(;;);
//	}
//	
//	cli_init();
	
	raw_task_suspend(raw_task_identify());	// 任务挂起, 等待sys_init唤醒
	for(;;)
	{
		raw_task_suspend(raw_task_identify());
//		cli_loop();
	}
}





void debug_uart_init(unsigned char prio)
{
	raw_task_create(&debug_shell_task_obj, 				/* 任务控制块地址 	*/
					(RAW_U8  *)"shell_daemon", 			/* 任务名 			*/
					(void *)0,							/* 任务参数 		*/
					prio, 								/* 优先级 			*/
					0,									/* 时间片 			*/
					debug_shell_task_stk,				/* 任务栈首地址 	*/
					DEBUG_SHELL_TASK_STK_SIZE ,			/* 任务栈大小 		*/
					debug_shell_task,					/* 任务入口地址 	*/
					1);									/* 是否立即运行 	*/	
}
