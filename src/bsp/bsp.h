#ifndef __BSP_H__
#define __BSP_H__

#include "raw_api.h"
#include <mm/raw_tlsf.h>

#define SDRAM_BASE_ADDR 	((void *)0xD0000000)
#define SDRAM_SIZE 			(64/8 *1024*1024)
#define AT_SDRAM 			__attribute__((section("SDRAM"),zero_init))

#define TLSF_INLINE
#define TLSF
#define PORT_MEMORY_SIZE 	(1 *1024*1024)
void port_memory_init(unsigned char prio);

#ifndef  TLSF_INLINE
void *port_malloc(unsigned int size);
void port_free(void *p);
void *port_realloc(void *p, unsigned int size);
#else
static inline void *port_malloc(unsigned int size)
{
	return tlsf_malloc(size);
}

static inline void port_free(void *p)
{
	if( !p ) return;
	tlsf_free(p);
}

static inline void *port_realloc(void *p, unsigned int newsize)
{
	return tlsf_realloc(p, newsize);
}
#endif

#ifndef offsetof
#define offsetof(type, member) ((unsigned int) &((type *)0)->member)
#endif

#ifndef container_of
#define container_of(ptr, type, member)  ((type *)( (char *)(ptr) - offsetof(type,member) ))
#endif

#endif
