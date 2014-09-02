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

#include 	<raw_api.h>

#if (CONFIG_RAW_USER_HOOK > 0)


RAW_VOID raw_os_init_hook()
{

}



RAW_VOID raw_task_delete_hook(RAW_TASK_OBJ *task_ptr)
{


}


RAW_VOID raw_task_abort_hook(RAW_TASK_OBJ *task_ptr)
{


}


RAW_VOID raw_idle_task_hook()
{


}


RAW_VOID raw_task_switch_hook()
{

	

}


RAW_VOID raw_tick_hook()
{



}


void task_create_hook(RAW_TASK_OBJ  *task_obj)
{
	
	
	
}

RAW_VOID raw_idle_coroutine_hook()
{

	while (1);

}


#endif



