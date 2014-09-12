#include <stm32f4xx.h>

#include <raw_api.h>

#include "serial_fifo.h"
#include "../cli/cli.h"

/******************************************************************************/
#define DEBUG_SHELL_TASK_STK_SIZE 	512
static  PORT_STACK 				debug_shell_task_stk[DEBUG_SHELL_TASK_STK_SIZE];
static  RAW_TASK_OBJ 			debug_shell_task_obj;
/******************************************************************************/

#define DEBUG_UART 		USART1

#define DEBUG_UART_CLK 			RCC_APB2Periph_USART1
#define DEBUG_UART_CLK_INIT 	RCC_APB2PeriphClockCmd

#define DEBUG_UART_IRQ 			USART1_IRQn
#define DEBUG_UART_IRQ_HANDLER 	USART1_IRQHandler

#define DEBUG_UART_AF 	GPIO_AF_USART1

#define DEBUG_UART_TXE 			(1<<7)
#define DEBUG_UART_RXNE 		(1<<5)

static struct
{
	GPIO_TypeDef *port;
	uint16_t pin;
}
DEBUG_UART_RX = {GPIOA, GPIO_Pin_10},
DEBUG_UART_TX = {GPIOA, GPIO_Pin_9 };


/******************************************************************************/

#define DEBUG_FIFO_DEPTH 			4
#define DEBUG_FIFO_BLOCK_SIZE 		16
DEFINE_FIFO(debug_rx, DEBUG_FIFO_DEPTH, DEBUG_FIFO_BLOCK_SIZE);

RAW_MUTEX debug_tx_mutex;

/******************************************************************************/



static void debug_uart_init(unsigned int baud)
{
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	GPIO_InitTypeDef GPIO_InitStructure;

	/* Enable GPIO clock */
//	RCC_AHB1PeriphClockCmd(USARTx_TX_GPIO_CLK | USARTx_RX_GPIO_CLK, ENABLE);

	/* Enable USART clock */
	DEBUG_UART_CLK_INIT(DEBUG_UART_CLK, ENABLE);

	/* Connect USART pins to AF7 */
	GPIO_PinAFConfig(DEBUG_UART_TX.port, DEBUG_UART_TX.pin, DEBUG_UART_AF);
	GPIO_PinAFConfig(DEBUG_UART_RX.port, DEBUG_UART_RX.pin, DEBUG_UART_AF);

	/* Configure USART Tx and Rx as alternate function push-pull */
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_InitStructure.GPIO_Pin = DEBUG_UART_TX.pin;
	GPIO_Init(DEBUG_UART_TX.port, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin = DEBUG_UART_RX.pin;
	GPIO_Init(DEBUG_UART_RX.port, &GPIO_InitStructure);


	/* Enable the USART OverSampling by 8 */
	USART_OverSampling8Cmd(DEBUG_UART, ENABLE);  

	/* USARTx configuration ----------------------------------------------------*/
	/* USARTx configured as follows:
		- BaudRate = 115200 baud
		- Word Length = 8 Bits
		- one Stop Bit
		- No parity
		- Hardware flow control disabled (RTS and CTS signals)
		- Receive and transmit enabled
	*/
	USART_StructInit(&USART_InitStructure);
	USART_InitStructure.USART_BaudRate = 115200;
	USART_Init(DEBUG_UART, &USART_InitStructure);

	/* NVIC configuration */
	/* Configure the Priority Group to 2 bits */
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);

	/* Enable the USARTx Interrupt */
	NVIC_InitStructure.NVIC_IRQChannel = DEBUG_UART_IRQ;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);

	/* Enable USART */
	USART_Cmd(DEBUG_UART, ENABLE);
}

static int debug_fifo_read(unsigned char *buf)
{
	int len = 0;
	
	if ( buf )
	{
		while ((DEBUG_UART->SR & DEBUG_UART_RXNE))
		{
			buf[len++] = (DEBUG_UART->DR & 0xFF);
		}
	}
	else
	{
		while ((DEBUG_UART->SR & DEBUG_UART_RXNE))
		{
			len = (DEBUG_UART->DR & 0xFF);
		}
		len = 0;
	}
	
	return len;
}

static inline void debug_uart_int_recv(void)
{
	serial_fifo_int_recv(&debug_rx_fifo, debug_fifo_read);
}

void DEBUG_UART_IRQ_HANDLER(void)
{
	NVIC_DisableIRQ(DEBUG_UART_IRQ);
	raw_enter_interrupt();
	
	/* USART in Receiver mode */
	if (USART_GetITStatus(DEBUG_UART, USART_IT_RXNE) == SET)
	{
		debug_uart_int_recv();
	}
	/* USART in Tramitter mode */
	if (USART_GetITStatus(DEBUG_UART, USART_IT_TXE) == SET)
	{

	}
	
	NVIC_EnableIRQ(DEBUG_UART_IRQ);
	raw_finish_int();
}

/******************************************************************************/

static void debug_fifo_init(void)
{
	RAW_U16 ret;
	
	ret =  raw_mutex_create(&debug_tx_mutex, (RAW_U8 *)"debug_tx_mutex", RAW_MUTEX_INHERIT_POLICY, 0);
	RAW_ASSERT(RAW_SUCCESS == ret);
	
	FIFO_INIT(debug_rx, DEBUG_FIFO_DEPTH, DEBUG_FIFO_BLOCK_SIZE);
}	

/**
 * 缓存为空返回 1
 */
int debug_cahce_empty(void)
{
	MSG_SIZE_TYPE msg[3];
	
	if(0 == debug_rx_fifo.count)
	{
		if ( RAW_SUCCESS != raw_queue_size_get_information(&debug_rx_fifo.fifo, msg, msg + 1, msg + 2))
		{
			RAW_ASSERT(0);
		}
		
		if( 0 == msg[2] )
		{
			return 1;
		}
	}
	
	return 0;
}

static inline void debug_putchar(unsigned char c)
{
	while( !(DEBUG_UART->SR & DEBUG_UART_TXE) );
	DEBUG_UART->DR = c;
}

void debug_putc(char c)
{
	debug_putchar(c);
}

int debug_write(const void *buff, int len)
{
	const unsigned char *buf = buff;
	int i;
	RAW_U16 ret;

	ret = raw_mutex_get(&debug_tx_mutex, RAW_WAIT_FOREVER);
	if( (RAW_SUCCESS != ret) && (RAW_MUTEX_OWNER_NESTED != ret) )
	{
		return 0;
	}
	
	for(i=0; i<len; i++)
	{
		debug_putchar(buf[i]);
	}
	
	ret = raw_mutex_put(&debug_tx_mutex);
	RAW_ASSERT(RAW_SUCCESS == ret);
	
	return i;
}

int debug_read(void *buff, int len)
{
	return serial_read(&debug_rx_fifo, buff, len);
}
int debug_read_at_least_one(void *buf, int len, RAW_TICK_TYPE wait_opt)
{
	return serial_read_at_least_one(&debug_rx_fifo, wait_opt, buf, len);
}
int debug_read_noblock(void *buf, int len, RAW_TICK_TYPE wait_opt)
{
	return serial_read_noblock(&debug_rx_fifo, wait_opt, buf, len);
}
/******************************************************************************/

RAW_TASK_OBJ *get_shell_task_obj(void)
{
	return &debug_shell_task_obj;
}

static void debug_shell_task(void *pdat)
{	
	(void)pdat;
	debug_uart_init(115200);
	debug_fifo_init();
	cli_init();
	
	raw_task_suspend(raw_task_identify());	// 任务挂起, 等待sys_init唤醒
	for(;;)
	{
		cli_loop();
	}
}

void debug_serial_init(unsigned char prio)
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
