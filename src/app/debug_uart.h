#ifndef __DEBUG_UART_H__
#define __DEBUG_UART_H__

#include "stm32f4xx.h"
#include "raw_api.h"

#define DEBUG_UART 				USART1

#define DEBUG_UART_CLK 			RCC_APB2Periph_USART1
#define DEBUG_UART_CLK_INIT 	RCC_APB2PeriphClockCmd

#define DEBUG_UART_IRQ 			USART1_IRQn
#define DEBUG_UART_IRQ_HANDLER 	USART1_IRQHandler

#define DEBUG_UART_AF 			GPIO_AF_USART1

#define DEBUG_UART_TXE 			(1<<7)
#define DEBUG_UART_RXNE 		(1<<5)

void debug_serial_init(void);
RAW_TASK_OBJ *get_shell_task_obj(void);
void shell_init(unsigned char prio);

static inline void debug_putchar(unsigned char c)
{
	while( !(DEBUG_UART->SR & DEBUG_UART_TXE) );
	DEBUG_UART->DR = c;
}

void debug_putc(char c);
int debug_write(const void *buff, int len);

int debug_read(void *buff, int len);
int debug_read_at_least_one(void *buf, int len, RAW_TICK_TYPE wait_opt);
int debug_read_noblock(void *buf, int len, RAW_TICK_TYPE wait_opt);

#endif
