#include <stm32f4xx.h>

#include <raw_api.h>

/******************************************************************************/
#define SYS_LED_TASK_STK_SIZE 	(768)
static PORT_STACK 				sys_led_task_stk[SYS_LED_TASK_STK_SIZE];
static RAW_TASK_OBJ 			sys_led_task_obj;
/******************************************************************************/

static struct
{
	GPIO_TypeDef *port;
	uint16_t pin;
}SYS_LED[] =
{
	{GPIOG, GPIO_Pin_13},
};

static void sys_len_init(void)
{
	int i;
	GPIO_InitTypeDef  GPIO_InitStructure;
	
	for(i=0; i<ARRAY_SIZE(SYS_LED); i++)
	{
		GPIO_InitStructure.GPIO_Pin = SYS_LED[i].pin;
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
		GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
		GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
		
		GPIO_Init(SYS_LED[i].port, &GPIO_InitStructure);
	}
}

static void sys_led_on(unsigned char n)
{
	SYS_LED[n].port->BSRRL = SYS_LED[n].pin;	//置位
}
static void sys_led_off(unsigned char n)
{
	SYS_LED[n].port->BSRRH = SYS_LED[n].pin; //复位
}

static void sys_led_task(void *pdat)
{
	(void)pdat;
	
	raw_printf("sys_led init...\r\t\t\t\t");
	sys_len_init();
	raw_printf("[OK]\r\n");
	
	for(;;)
	{
		sys_led_on(0);
		raw_sleep(RAW_TICKS_PER_SECOND>>1);
		sys_led_off(0);
		raw_sleep(RAW_TICKS_PER_SECOND>>1);
	}
}

void sys_led_init(RAW_U8 prio)
{
	raw_task_create(&sys_led_task_obj, 			/* 任务控制块地址 	*/
					(RAW_U8  *)"sys_led_task", 	/* 任务名 			*/
					(void *)0,					/* 任务参数 		*/
					prio, 						/* 优先级 			*/
					0,							/* 时间片 			*/
					sys_led_task_stk,			/* 任务栈首地址 	*/
					SYS_LED_TASK_STK_SIZE ,		/* 任务栈大小 		*/
					sys_led_task,				/* 任务入口地址 	*/
					1);							/* 是否立即运行 	*/
}
