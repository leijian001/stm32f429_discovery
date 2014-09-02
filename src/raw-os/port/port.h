/*
     raw os - Copyright (C)  Lingjun Chen(jorya_txj).

    This file is part of raw os.

    raw os is free software; you can redistribute it it under the terms of the 
    GNU General Public License as published by the Free Software Foundation; 
    either version 3 of the License, or  (at your option) any later version.

    raw os is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; 
    without even the implied warranty of  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  
    See the GNU General Public License for more details.

    You should have received a copy of the GNU Lesser General Public License
    along with this program. if not, write email to jorya.txj@gmail.com
                                      ---

    A special exception to the LGPL can be applied should you wish to distribute
    a combined work that includes raw os, without being obliged to provide
    the source code for any proprietary components. See the file exception.txt
    for full details of how and when the exception can be applied.
*/


/* 	2012-8  Created by jorya_txj
  *	xxxxxx   please added here
  */

#ifndef PORT_H
#define PORT_H


/*function prototype for task context switch during interrupt*/
void raw_int_switch(void);
RAW_VOID  *port_stack_init(PORT_STACK  *p_stk_base, RAW_U32 stk_size,  RAW_VOID   *p_arg, RAW_TASK_ENTRY p_task);
void port_task_switch(void);
void raw_start_first_task(void);

void raw_sys_tick_init(void);

extern void raw_printk(const char *p_fmt, ...);

//#define  RAW_ASSERT(CON)        if (!(CON)) {volatile RAW_U8 dummy = 0; raw_printk("file is %s, line is %d\n",__FILE__, __LINE__);raw_printk("aborted task is %s\n", raw_task_active->task_name); while (dummy==0);}

#define  RAW_ASSERT(CON)        if (!(CON)) {volatile RAW_U8 dummy = 0; while (dummy == 0);}


#define  CONTEXT_SWITCH()    	port_task_switch();          


#if (CONFIG_RAW_USER_HOOK > 0)

RAW_VOID raw_os_init_hook(void);

RAW_VOID task_create_hook(RAW_TASK_OBJ *task_ptr);

RAW_VOID raw_task_delete_hook(RAW_TASK_OBJ *task_ptr);

RAW_VOID raw_task_abort_hook(RAW_TASK_OBJ *task_ptr);

RAW_VOID raw_idle_task_hook(void);

RAW_VOID raw_task_switch_hook(void);

RAW_VOID raw_tick_hook(void);

RAW_VOID raw_idle_coroutine_hook(void);

#endif


#endif


