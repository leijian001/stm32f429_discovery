#include <stm32f4xx.h>
#include <bsp.h>
#include "stm32f429i_discovery_sdram.h"
#include "stm32f429i_discovery_lcd.h"

#include <mm/raw_tlsf.h>

#include <raw_api.h>

#include <string.h>

static unsigned int port_memory_pool[PORT_MEMORY_SIZE / sizeof(unsigned int)] AT_SDRAM;

#ifndef TLSF
#define MEM_ALIGN 		4
#define ROUNDUP_SIZE(x) ( ((x)+MEM_ALIGN-1) & ~(MEM_ALIGN-1) )
typedef struct
{
	unsigned int size;
	unsigned char buf[];
}mem_t;
static RAW_BYTE_POOL_STRUCT port_momory;
#endif

void port_memory_init(unsigned char prio)
{
	RAW_U32 ret;
	
	raw_printf("TLSF RAM init...\r\t\t\t\t");
#ifdef TLSF
	port_memory_pool[0] = 0;
	ret = init_memory_pool(PORT_MEMORY_SIZE, port_memory_pool);
	RAW_ASSERT((RAW_U32)-1 != ret);
#else
	ret = raw_byte_pool_create(&port_momory, (RAW_U8*)"port_memory", port_memory_pool, PORT_MEMORY_SIZE);
	RAW_ASSERT(RAW_SUCCESS == ret);
#endif
	raw_printf("[OK]\n");
}

#ifndef TLSF_INLINE
void *port_malloc(unsigned int size)
{
#ifdef TLSF
	return tlsf_malloc(size);
#else
	RAW_U16 ret;
	mem_t *ptr;
	size = ROUNDUP_SIZE(size);
	ret = raw_byte_allocate(&port_momory, (void **)&ptr, sizeof(mem_t)+size);
	if(RAW_SUCCESS != ret)
	{
		return NULL;
	}
	ptr->size = size;
	return ptr->buf;
#endif
}

void port_free(void *p)
{
#ifdef TLSF
	tlsf_free(p);
#else
	if( ! p )
	{
		return;
	}
	RAW_U16 ret;
	mem_t *ptr = container_of(p, mem_t, buf);
	ret = raw_byte_release(&port_momory, ptr);
	RAW_ASSERT(RAW_SUCCESS == ret);
#endif
}

void *port_realloc(void *p, unsigned int newsize)
{
#ifdef TLSF
	return tlsf_realloc(p, newsize);
#else
	if( p )
	{
		mem_t *ptr = container_of(p, mem_t, buf);
		unsigned char *dst;
		if( ptr->size >= newsize )
		{
			return p;
		}
		else
		{
			dst = port_malloc(newsize);
			if( ! dst )
			{
				return NULL;
			}
			memcpy(dst, p, ptr->size);
			port_free(p);
			return dst;
		}
	}
	else
	{
		if( newsize )
			return port_malloc(newsize);
		else
			return NULL;
			
	}
#endif
}
#endif

void bsp_init(void)
{
	SystemCoreClockUpdate();
	
	/* NVIC configuration */
	/* Configure the Priority Group to 2 bits */
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
	
	// 使能 GPIOG 的时钟
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA | RCC_AHB1Periph_GPIOB | RCC_AHB1Periph_GPIOC |
							RCC_AHB1Periph_GPIOD | RCC_AHB1Periph_GPIOE | RCC_AHB1Periph_GPIOF |
							RCC_AHB1Periph_GPIOG, ENABLE);
	
	SDRAM_Init();
}
