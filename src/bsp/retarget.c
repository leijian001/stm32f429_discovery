#include <stdio.h>
#include <stdlib.h>
#include <rt_misc.h>

#pragma import(__use_no_semihosting_swi)

struct __FILE 
{
	int handle;
	/* Add whatever you need here */
};
FILE __stdout;
FILE __stdin;


int fputc(int c, FILE *f)
{
	return 0;
}


int fgetc(FILE *f)
{
	return EOF;
}


int ferror(FILE *f) 
{
	/* Your implementation of ferror */
	return EOF;
}


void _ttywrch(int c)
{
	return;
}

int errorno = 0;
void _sys_exit(int return_code)
{
loop:
	errorno = return_code;
	goto loop;
}
