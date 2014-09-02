#include <stm32f4xx.h>
#include <bsp.h>
#include "stm32f429i_discovery_sdram.h"
#include "stm32f429i_discovery_lcd.h"

#include <mm/raw_tlsf.h>

#include <raw_api.h>

static unsigned int port_memory_pool[PORT_MEMORY_SIZE / sizeof(unsigned int)] AT_SDRAM;

void port_memory_init(unsigned char prio)
{
	RAW_U32 ret;
	ret = init_memory_pool(PORT_MEMORY_SIZE, port_memory_pool);
	RAW_ASSERT((RAW_U32)-1 != ret);
}

void *port_malloc(unsigned int size)
{
	return tlsf_malloc(size);
}

void port_free(void *p)
{
	tlsf_free(p);
}

void *port_realloc(void *p, unsigned int size)
{
	return tlsf_realloc(p, size);
}

void bsp_init(void)
{
	SystemCoreClockUpdate();
	
	// 使能 GPIOG 的时钟
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA | RCC_AHB1Periph_GPIOB | RCC_AHB1Periph_GPIOC |
							RCC_AHB1Periph_GPIOD | RCC_AHB1Periph_GPIOE | RCC_AHB1Periph_GPIOF |
							RCC_AHB1Periph_GPIOG, ENABLE);
	
	SDRAM_Init();
	LCD_Init();
	LCD_LayerInit();
	/* Enable The LCD */
	LTDC_Cmd(ENABLE);
}
