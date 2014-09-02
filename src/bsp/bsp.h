#ifndef __BSP_H__
#define __BSP_H__

#define SDRAM_BASE_ADDR 	((void *)0xD0000000)
#define SDRAM_SIZE 			(64/8 *1024*1024)
#define AT_SDRAM 	__attribute__((section("SDRAM")))

#define PORT_MEMORY_SIZE 	(64 *1024)
void port_memory_init(unsigned char prio);
void *port_malloc(unsigned int size);
void port_free(void *p);
void *port_realloc(void *p, unsigned int size);


#endif
