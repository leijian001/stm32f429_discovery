#include "stm32f4xx.h"

#include "raw_api.h"

#include "GUI.h"
#include "DIALOG.h"


/******************************************************************************/
#define GUI_TASK_STK_SIZE 	(1024)
static PORT_STACK 				gui_task_stk[GUI_TASK_STK_SIZE];
static RAW_TASK_OBJ 			gui_task_obj;
/******************************************************************************/

WM_HWIN CreateWindow(void);

static void gui_task(void *pdat)
{
	(void)pdat;
	/* Enable the CRC Module */
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_CRC, ENABLE); 

	GUI_Init();
	
	CreateWindow();
	GUI_Exec();
	
	for(;;)
	{
		raw_sleep(1);
	}
}




void gui_init(RAW_U8 prio)
{
	raw_task_create(&gui_task_obj, 				/* 任务控制块地址 	*/
					(RAW_U8  *)"GUI_task", 		/* 任务名 			*/
					(void *)0,					/* 任务参数 		*/
					prio, 						/* 优先级 			*/
					0,							/* 时间片 			*/
					gui_task_stk,				/* 任务栈首地址 	*/
					GUI_TASK_STK_SIZE ,			/* 任务栈大小 		*/
					gui_task,					/* 任务入口地址 	*/
					1);							/* 是否立即运行 	*/
}



