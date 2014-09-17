#include <raw_api.h>

#include <bsp.h>

/******************************************************************************/
#define SYS_INIT_TASK_STK_SIZE 	(512)
static PORT_STACK 				sys_init_task_stk[SYS_INIT_TASK_STK_SIZE];
static RAW_TASK_OBJ 			sys_init_task_obj;
/******************************************************************************/

//#define MEMORY_TEST

typedef void (*init_func_t)(RAW_U8 prio);
typedef struct init_task_t
{
	init_func_t init_task;	//任务初始化函数
	RAW_U8 prio;			//任务优先级
}init_task_t;

/******************************************************************************/
#include "debug_uart.h"

void sys_led_init(RAW_U8 prio);
void fatfs_init(unsigned char prio);
void lua_task_init(unsigned char prio);

/******************************************************************************/

static const init_task_t sys_init_arry[]  = 
{
	{port_memory_init, 		CONFIG_RAW_PRIO_MAX     }, 		// 总堆栈
	{sys_led_init, 			CONFIG_RAW_PRIO_MAX - 3 }, 		// 系统指示灯
	{fatfs_init, 			CONFIG_RAW_PRIO_MAX 	}, 		// FatFS文件系统初始化
	{lua_task_init, 		CONFIG_RAW_PRIO_MAX - 5 }, 		// 一个app
	{shell_init, 			CONFIG_RAW_PRIO_MAX - 10},		// shell
};

#ifdef MEMORY_TEST
static int memory_test(void)
{
	unsigned int *const sdram = SDRAM_BASE_ADDR;
	unsigned int i=0;
	
	raw_printf("SDRAM test...\r\t\t\t\t");
	for(i=0; i<SDRAM_SIZE/sizeof(unsigned int); i++)
	{
		sdram[i] = i;
	}
	
	for(i=0; i<SDRAM_SIZE/sizeof(unsigned int); i++)
	{
		if(sdram[i] != i)
			break;
	}
	
	if(i>=SDRAM_SIZE/sizeof(unsigned int))
	{
		raw_printf("[OK]\n");
		return 0;
	}
	else
	{
		raw_printf("[Failed]\n");
		return -1;
	}
}
#endif

static void sys_init_task(void *pdat)
{
	(void)pdat;
	int i;
	
	raw_sys_tick_init();   //第一个任务启动时开启系统tick定时器
	
	// debug_uart 初始化
	debug_serial_init();
	
	raw_printf("\r\n-------------  raw-os  ----------------\r\n");

#ifdef MEMORY_TEST	
	RAW_ASSERT( 0 == memory_test() );
#endif
	
	// 任务初始化
	for(i=0; i<ARRAY_SIZE(sys_init_arry); i++)
	{
		sys_init_arry[i].init_task( sys_init_arry[i].prio );
	}

	raw_printf("\r\n---------------------------------------\r\n");
	
	raw_task_resume(get_shell_task_obj()); 		// 唤醒shell_deamon
	
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
