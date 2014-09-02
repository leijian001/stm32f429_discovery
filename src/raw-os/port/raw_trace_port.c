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


/* 	2013-4  Created by jorya_txj
  *	xxxxxx   please added here
  */



#include    <raw_api.h>



#if (CONFIG_RAW_TRACE_ENABLE > 0)

void _trace_init()
{
	

}

void _trace_task_switch(RAW_TASK_OBJ *from, RAW_TASK_OBJ *to)
{
		from = from;
		to = to;		
}

void _trace_int_task_switch(RAW_TASK_OBJ *from, RAW_TASK_OBJ *to)
{
		
	from = from;
	to = to;
}


void _trace_task_create(RAW_TASK_OBJ *task)
{
	task = task;

}


void _trace_task_priority_change(RAW_TASK_OBJ *task, RAW_U8 priority)
{

	task = task;
	priority = priority;

}


void _trace_task_suspend(RAW_TASK_OBJ *task_obj, RAW_TASK_OBJ *task)
{
	task_obj = task_obj;
	task = task;
}


void _trace_task_resume(RAW_TASK_OBJ *task_obj, RAW_TASK_OBJ *task_resumed)
{

	task_obj = task_obj;
	task_resumed = task_resumed;
	
}


void _trace_task_delete(RAW_TASK_OBJ *task)
{

	task = task;

}


void _trace_task_abort(RAW_TASK_OBJ *task)
{

	task = task;

}


void _trace_task_0_overflow(EVENT_HANLDER *p, TASK_0_EVENT_TYPE ev, void *event_data)
{

	p = p;
	ev = ev;
	event_data = event_data;

}

void _trace_int_msg_exhausted()
{



}


void _trace_int_msg_post(RAW_U8 type, void *p_obj, void *p_void, RAW_U32 msg_size, RAW_U32 flags, RAW_U8 opt)
{

	type = type;
	p_obj = p_obj;
	p_void = p_void;
	msg_size = msg_size;
	flags = flags;
	opt = opt;

}

void _trace_semaphore_create(RAW_TASK_OBJ *task, RAW_SEMAPHORE *semaphore_obj)
{

	task = task;
	semaphore_obj = semaphore_obj;

}



void _trace_semaphore_overflow(RAW_TASK_OBJ *task, RAW_SEMAPHORE *semaphore_obj)
{
	task = task;
	semaphore_obj = semaphore_obj;
  


}

void _trace_semaphore_delete(RAW_TASK_OBJ *task, RAW_SEMAPHORE *semaphore_obj)
{

	task = task;
	semaphore_obj = semaphore_obj;

}


void _trace_semaphore_get_success(RAW_TASK_OBJ *task, RAW_SEMAPHORE *semaphore_obj)
{

	task = task;
	semaphore_obj = semaphore_obj;

}


void _trace_semaphore_get_block(RAW_TASK_OBJ *task, RAW_SEMAPHORE *semaphore_obj, RAW_TICK_TYPE wait_option)
{

	task = task;
	semaphore_obj = semaphore_obj;
	wait_option = wait_option;

}

void _trace_sem_wake_task(RAW_TASK_OBJ *task, RAW_TASK_OBJ *task_waked_up, RAW_U8 opt_wake_all)
{

	task = task;
	task_waked_up = task_waked_up;
	opt_wake_all = opt_wake_all;
}


void _trace_semaphore_count_increase(RAW_TASK_OBJ *task, RAW_SEMAPHORE *semaphore_obj)
{

	task = task;
	semaphore_obj = semaphore_obj;

}


void _trace_queue_create(RAW_TASK_OBJ *task, RAW_QUEUE *queue_obj)
{

	task = task;
	queue_obj = queue_obj;

}


void _trace_queue_msg_post(RAW_TASK_OBJ *task, RAW_QUEUE *queue_obj, void *msg, RAW_U8 opt_send_method)

{
	task = task;
	queue_obj = queue_obj;
	msg = msg;
	(void)opt_send_method;

}

void _trace_queue_wake_task(RAW_TASK_OBJ *task, RAW_TASK_OBJ *task_waked_up, void *msg, RAW_U8 opt_wake_all)
{
	task = task;
	task_waked_up = task_waked_up;
	msg = msg;
	opt_wake_all = opt_wake_all;
}



void  _trace_queue_get_msg(RAW_TASK_OBJ *task, RAW_QUEUE *queue_obj, RAW_TICK_TYPE wait_option, void *msg)
{

	task = task;
	queue_obj = queue_obj;
	wait_option = wait_option;
	msg = msg;
}


void _trace_queue_msg_max(RAW_TASK_OBJ *task, RAW_QUEUE *queue_obj, void *msg, RAW_U8 opt_send_method)
{

	task = task;
	queue_obj = queue_obj;
	msg = msg;
	opt_send_method = opt_send_method;


}


void _trace_queue_get_block(RAW_TASK_OBJ *task, RAW_QUEUE *queue_obj, RAW_TICK_TYPE wait_option)
{

	task = task;
	queue_obj = queue_obj;
	wait_option = wait_option;


}


void _trace_queue_flush(RAW_TASK_OBJ *task, RAW_QUEUE *queue_obj)
{

	task = task;
	queue_obj = queue_obj;


}


void _trace_queue_delete(RAW_TASK_OBJ *task, RAW_QUEUE *queue_obj)
{

	task = task;
	queue_obj = queue_obj;

}


void _trace_mutex_create(RAW_TASK_OBJ *task,RAW_MUTEX *mutex_ptr, RAW_U8 *name_ptr, RAW_U8 policy, RAW_U8 ceiling_prio)
{

	task = task;
	mutex_ptr = mutex_ptr;
	name_ptr = name_ptr;
	policy = policy;
	ceiling_prio = ceiling_prio;


}


void _trace_mutex_release(RAW_TASK_OBJ *task, RAW_TASK_OBJ *tcb, RAW_U8 newpri)
{
	task = task;
	tcb = tcb;
	newpri = newpri;

}


void _trace_mutex_ex_ce_pri(RAW_TASK_OBJ *task, RAW_MUTEX *mutex_obj, RAW_TICK_TYPE wait_option)
{

	task = task;
	mutex_obj = mutex_obj;
	wait_option = wait_option;
}


void _trace_mutex_get(RAW_TASK_OBJ *task, RAW_MUTEX *mutex_obj, RAW_TICK_TYPE wait_option)
{

	task = task;
	mutex_obj = mutex_obj;
	wait_option = wait_option;
}

void _trace_task_pri_inv(RAW_TASK_OBJ *task, RAW_TASK_OBJ *mtxtsk)
{
	task = task;
	mtxtsk = mtxtsk;

}



void _trace_mutex_get_block(RAW_TASK_OBJ *task, RAW_MUTEX *mutex_obj, RAW_TICK_TYPE wait_option)
{

	task = task;
	mutex_obj = mutex_obj;
	wait_option = wait_option;

}

void _trace_mutex_release_success(RAW_TASK_OBJ *task, RAW_MUTEX *mutex_obj)
{

	task = task;
	mutex_obj = mutex_obj;
}

void _trace_mutex_wake_task(RAW_TASK_OBJ *task, RAW_TASK_OBJ *task_waked_up)
{
	task = task;
	task_waked_up = task_waked_up;
}


void _trace_mutex_delete(RAW_TASK_OBJ *task, RAW_MUTEX *mutex_obj)
{
	task = task;
	mutex_obj = mutex_obj;

}

void _trace_task_stack_space(RAW_TASK_OBJ *task_obj)
{

	task_obj = task_obj;

}


void _trace_block_no_memory(RAW_TASK_OBJ *task_obj, MEM_POOL *pool_obj)
{

	task_obj = task_obj;
	pool_obj = pool_obj;
	
}


void _trace_block_pool_create(RAW_TASK_OBJ *task_obj, MEM_POOL *pool_obj)
{

	task_obj = task_obj;
	pool_obj = pool_obj;

}


void _trace_byte_no_memory(RAW_TASK_OBJ *task_obj, RAW_BYTE_POOL_STRUCT *pool_obj)
{

	task_obj = task_obj;
	pool_obj = pool_obj;

}


void _trace_event_create(RAW_TASK_OBJ *task_obj, RAW_EVENT *event_obj, RAW_U8 *name, RAW_U32 flags_init)
{

	task_obj = task_obj;
	event_obj = event_obj;
	name = name;
	flags_init = flags_init;
}


void _trace_event_get(RAW_TASK_OBJ *task_obj, RAW_EVENT *event_obj)
{

	task_obj = task_obj;
	event_obj = event_obj;

}


void _trace_event_get_block(RAW_TASK_OBJ *task_obj, RAW_EVENT *event_obj, RAW_TICK_TYPE wait_option)
{

	task_obj = task_obj;
	event_obj = event_obj;
	wait_option = wait_option;
	
}


void _trace_event_wake(RAW_TASK_OBJ *task_obj, RAW_TASK_OBJ *task)
{

	task_obj = task_obj;
	task = task;

}


void _trace_event_delete(RAW_TASK_OBJ *task_obj, RAW_EVENT *event_obj)
{

	task_obj = task_obj;
	event_obj = event_obj;

}


void _trace_queue_size_create(RAW_TASK_OBJ *task_obj, RAW_QUEUE_SIZE *queue_size_obj)
{

	task_obj = task_obj;
	queue_size_obj = queue_size_obj;

}


void _trace_queue_size_msg_max(RAW_TASK_OBJ *task_obj, RAW_QUEUE_SIZE *queue_size_obj, void *msg, RAW_U32 msg_size, RAW_U8 opt_send_method)
{

	task_obj = task_obj;
	queue_size_obj = queue_size_obj;
	msg = msg;
	msg_size = msg_size;
	opt_send_method = opt_send_method;

}

void _trace_queue_size_msg_post(RAW_TASK_OBJ *task_obj, RAW_QUEUE_SIZE *queue_size_obj, void *msg, RAW_U32 msg_size, RAW_U8 opt_send_method)
{

	task_obj = task_obj;
	queue_size_obj = queue_size_obj;
	msg = msg;
	msg_size = msg_size;
	opt_send_method = opt_send_method;
	
}


void _trace_queue_size_wake_task(RAW_TASK_OBJ *task_obj, RAW_TASK_OBJ *task_waked_up, void *msg, RAW_U32 msg_size, RAW_U8 opt_wake_all)
{
	task_obj = task_obj;
	task_waked_up = task_waked_up;
	msg = msg;
	msg_size = msg_size;
	opt_wake_all = opt_wake_all;
}


void _trace_queue_size_get_msg(RAW_TASK_OBJ *task_obj, RAW_QUEUE_SIZE *queue_size_obj, RAW_TICK_TYPE wait_option, void *msg, RAW_U32 msg_size)
{
	task_obj = task_obj;
	queue_size_obj = queue_size_obj;
	wait_option = wait_option;
	msg = msg;
	msg_size = msg_size;

}

void _trace_queue_size_get_block(RAW_TASK_OBJ *task_obj, RAW_QUEUE_SIZE *queue_size_obj, RAW_TICK_TYPE wait_option)
{
	task_obj = task_obj;
	queue_size_obj = queue_size_obj;
	wait_option = wait_option;

}

void _trace_queue_size_flush(RAW_TASK_OBJ *task_obj, RAW_QUEUE_SIZE *queue_size_obj)
{

	task_obj = task_obj;
	queue_size_obj = queue_size_obj;

}

void _trace_queue_size_delete(RAW_TASK_OBJ *task_obj, RAW_QUEUE_SIZE *queue_size_obj)
{

	task_obj = task_obj;
	queue_size_obj = queue_size_obj;

}


#endif

