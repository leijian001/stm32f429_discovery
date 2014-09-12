#ifndef __CLI_H__
#define __CLI_H__

#include "cli_config.h"

struct cli_io_t
{
	int (*putc)(char c);
	int (*puts)(const char *str);
	int (*putnstr)(const char *str, int n); 
	int (*getc)(void);
	int (*printf)(const char *fmt, ...);
	
	void *(*malloc)(unsigned int size);
	void (*free)(void *p);
};
extern struct cli_io_t __cli_io;

void cli_init(void);
void cli_loop(void);
int run_command_repeatable(const char *cmd, int flag);


#endif
