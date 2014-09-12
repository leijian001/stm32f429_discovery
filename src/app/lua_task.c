#include <stdio.h>

#include <raw_api.h>

#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>

/******************************************************************************/
#define LUA_TASK_STK_SIZE 		(16 *1024)
static PORT_STACK 				lua_task_stk[LUA_TASK_STK_SIZE];
static RAW_TASK_OBJ 			lua_task_obj;
/******************************************************************************/

#include "stm32f429i_discovery_lcd.h"

static void lua_task(void *pdat)
{
	(void)pdat;
	
	lua_State *L = luaL_newstate();
	luaL_openlibs(L);
	
	for(;;)
	{
		LCD_Clear(0xFFFF);
		LCD_SetLayer(LCD_FOREGROUND_LAYER);
		LCD_DrawCircle(100, 100, 50);
		
		unsigned short *const frame = lcd_get_frame_base();
		for(int i=0; i<1000; i++)
		{
			frame[i] = LCD_COLOR_RED;
		}
		
		raw_task_suspend(raw_task_identify());
	}
}

void lua_task_init(unsigned char prio)
{
	raw_task_create(&lua_task_obj,			/* 任务控制块地址 	*/
					(RAW_U8  *)"lua_task",	/* 任务名 			*/
					(void *)0,				/* 任务参数 		*/
					prio,					/* 优先级 			*/
					0,						/* 时间片 			*/
					lua_task_stk,			/* 任务栈首地址 	*/
					LUA_TASK_STK_SIZE ,		/* 任务栈大小 		*/
					lua_task,				/* 任务入口地址 	*/
					1);						/* 是否立即运行 	*/
}
