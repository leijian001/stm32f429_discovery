#include <raw_api.h>

#include <bsp.h>

/******************************************************************************/
#define SYS_INIT_TASK_STK_SIZE 	128
static PORT_STACK 				sys_init_task_stk[SYS_INIT_TASK_STK_SIZE];
static RAW_TASK_OBJ 			sys_init_task_obj;
/******************************************************************************/

typedef void (*init_func_t)(RAW_U8 prio);
typedef struct init_task_t
{
	init_func_t init_task;	//任务初始化函数
	RAW_U8 prio;			//任务优先级
}init_task_t;

/******************************************************************************/

void debug_uart_init(unsigned char prio);
RAW_TASK_OBJ *get_shell_task_obj(void);

void sys_led_init(RAW_U8 prio);

/******************************************************************************/

static const init_task_t sys_init_arry[]  = 
{
	{debug_uart_init, 		CONFIG_RAW_PRIO_MAX - 10},		// 核心板串口
	{port_memory_init, 		CONFIG_RAW_PRIO_MAX     },
	{sys_led_init, 			CONFIG_RAW_PRIO_MAX - 3 },
};

#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>

static void sys_init_task(void *pdat)
{
	(void)pdat;
	int i;
	
	raw_sys_tick_init();   //第一个任务启动时开启系统tick定时器
	
	raw_printf("\r\n-------------  raw-os  ----------------\r\n");
	
	// debug_uart 初始化
	sys_init_arry[0].init_task( sys_init_arry[0].prio );
	
	// 任务初始化, 从1开始，跳过debug_uart
	for(i=1; i<ARRAY_SIZE(sys_init_arry); i++)
	{
		sys_init_arry[i].init_task( sys_init_arry[i].prio );
	}

	raw_printf("\r\n---------------------------------------\r\n");
	
	raw_task_resume(get_shell_task_obj()); 		// 唤醒shell_deamon
	
	lua_State *L = luaL_newstate();
	luaL_openlibs(L);
	
	for(;;)
	{
		
		raw_task_suspend(raw_task_identify());
	}
}

void create_init_task(void)
{
	raw_task_create(&sys_init_task_obj,			/* 任务控制块地址 	*/
					(RAW_U8  *)"sys_init_task",	/* 任务名 			*/
					(void *)0,					/* 任务参数 		*/
					CONFIG_RAW_PRIO_MAX - 2,	/* 优先级 			*/
					0,							/* 时间片 			*/
					sys_init_task_stk,			/* 任务栈首地址 	*/
					SYS_INIT_TASK_STK_SIZE ,	/* 任务栈大小 		*/
					sys_init_task,				/* 任务入口地址 	*/
					1);							/* 是否立即运行 	*/
}	
