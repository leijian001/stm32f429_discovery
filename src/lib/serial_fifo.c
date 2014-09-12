#include <string.h>
#include "raw_api.h"
#include "serial_fifo.h"


#ifndef NULL
#define NULL 			((void *)0)
#endif

#define SERIAL_NULL 	((void *)0xFFFFFFFF)

#define SERIAL_ASSERT(cond) 	do{ volatile unsigned char _true = 1; if( !(cond) ) while(_true); }while(0)

void serial_fifo_init(serial_fifo_t *fifo, 
						const char *fifo_pool_name,		// FIFO缓冲池名称
						RAW_MSG_SIZE *fifo_pool, 		// FIFO缓冲池地址
						unsigned int fifo_depth, 		// FIFO缓冲深度
						const char *cache_pool_name, 	// block内存池名称
						unsigned char *cache_pool, 		// block内存池地址
						unsigned int block_size, 		// block内存块大小
						const char *mutex_name)			// mutex名称
{
	RAW_U16 ret;
	
	if( NULL == fifo 
		|| ((unsigned int)fifo_pool & 0x03) 		// 四字节对齐?
		|| ((unsigned int)cache_pool & 0x03) )		// 四字节对齐?
	{
		SERIAL_ASSERT(0);
	}
	
	fifo->stream = NULL;
	fifo->count  = 0;
	fifo->pos    = NULL;
	
	fifo->fifo_pool  = fifo_pool;
	fifo->cache_pool = cache_pool;
	
	fifo->fifo_depth = fifo_depth;
	fifo->cache_block_size = block_size;
	
	ret = raw_queue_size_create(&fifo->fifo, (RAW_U8 *)fifo_pool_name, fifo->fifo_pool, fifo_depth);
	RAW_ASSERT( RAW_SUCCESS == ret );
	
	ret = raw_block_pool_create(&fifo->cache, (RAW_U8 *)cache_pool_name, 
				block_size, fifo->cache_pool, fifo_depth * block_size);
	RAW_ASSERT( RAW_SUCCESS == ret );
	
	ret = raw_mutex_create(&fifo->mutex, (RAW_U8 *)mutex_name, RAW_MUTEX_INHERIT_POLICY, 0);
	RAW_ASSERT( RAW_SUCCESS == ret );
}

/******************************************************************************/

static inline int copy2buf(serial_fifo_t *fifo, unsigned char **buf, int len)
{
	int i;
	
	for(i=0; i<len; i++)
	{
		*(*buf)++ = *fifo->pos++;
		fifo->count--;
	}
	
	return i;
}

static inline int serial_fifo_cache_refresh(serial_fifo_t *fifo, RAW_TICK_TYPE wait_opt)
{
	RAW_U16 ret;
	void *tmp;
	unsigned int len;

	if(NULL != fifo->stream)
	{
		ret = raw_block_release(&fifo->cache, fifo->stream);
		SERIAL_ASSERT(RAW_SUCCESS == ret);
	}
	
	ret = raw_queue_size_receive(&fifo->fifo, wait_opt, &tmp, &len);
	if(RAW_SUCCESS != ret || SERIAL_NULL == tmp)
	{
		goto failed;
	}
	
	fifo->stream = tmp;
	fifo->pos    = tmp;
	fifo->count  = len;
	
	return 0;
failed:
	fifo->stream = NULL;
	fifo->pos    = NULL;
	fifo->count  = 0;
	
	return -1;
}

static inline int serial_fifo_current_size(serial_fifo_t *fifo)
{
	return fifo->count;
}

// 为空返回真
static inline int serial_fifo_current_is_empty(serial_fifo_t *fifo)
{
	return !(fifo->count);
}

/******************************************************************************/

int serial_read_noblock(serial_fifo_t *fifo, RAW_TICK_TYPE wait_opt, void *buff, int len)
{
	unsigned char *buf = buff;
	int n;
	int count;
	
	RAW_U16 ret;
	ret = raw_mutex_get(&fifo->mutex, wait_opt);
	if(RAW_SUCCESS != ret && RAW_MUTEX_OWNER_NESTED != ret)
	{
		return -1;
	}
	
	n = 0;
	
	if( serial_fifo_current_size(fifo) >= len)
	{
		n += copy2buf(fifo, &buf, len);
	}
	else
	{
		for(n=0; n<len; )
		{
			if( serial_fifo_current_is_empty(fifo) )
			{
				if( serial_fifo_cache_refresh(fifo, wait_opt) < 0 )
				{
					break;
				}
			}
			
			count = len - n; 	// 需要copy的
			// 能copy的
			count = (count <= serial_fifo_current_size(fifo)) ? count : serial_fifo_current_size(fifo);
			n += copy2buf(fifo, &buf, count);
		}
	}
	
	raw_mutex_put(&fifo->mutex);
	
	return n;
}

inline int serial_read(serial_fifo_t *fifo, void *buff, int len)
{
	return serial_read_noblock(fifo, RAW_WAIT_FOREVER, buff, len);
}

inline int serial_read_at_least_one(serial_fifo_t *fifo, RAW_TICK_TYPE wait_opt, void *buff, int len)
{
	int n;
	
	RAW_U16 ret;
	ret = raw_mutex_get(&fifo->mutex, wait_opt);
	if(RAW_SUCCESS != ret && RAW_MUTEX_OWNER_NESTED != ret)
	{
		return -1;
	}
	
	n = 0;
	n += serial_read(fifo, buff, 1);
	n += serial_read_noblock(fifo, wait_opt, (unsigned char *)buff + 1, len - 1);
	
	raw_mutex_put(&fifo->mutex);
	
	return n;
}

inline int serial_getchar(serial_fifo_t *fifo)
{
	int ret;
	unsigned char tmp;
	
	ret = serial_read_noblock(fifo, RAW_WAIT_FOREVER, &tmp, 1);
	
	return (ret < 1)? 0 : tmp;
}

/******************************************************************************/

void serial_fifo_read_abort(serial_fifo_t *fifo)
{
	if( RAW_SUCCESS != raw_queue_size_front_post(&fifo->fifo, SERIAL_NULL, 0))
	{
		SERIAL_ASSERT(0);
	}
}

void serial_fifo_flush(serial_fifo_t *fifo)
{
	while( ! serial_fifo_cache_refresh(fifo, RAW_NO_WAIT) );
}

/******************************************************************************/

int serial_append(serial_fifo_t *fifo, void *buf, int len)
{
	return raw_queue_size_end_post(&fifo->fifo, buf, len) == RAW_SUCCESS ?  0 : -1;
}
void *serial_alloc(serial_fifo_t *fifo)
{
	void *tmp = 0; //raw_block_release(MEM_POOL *pool_ptr, RAW_VOID *block_ptr)
	return ( RAW_SUCCESS == raw_block_allocate(&fifo->cache, &tmp) )?
			tmp : 0;
}

/**
 * 将buf中len个字节加到FIFO中
 */
int serial_fifo_append(serial_fifo_t *fifo, const void *buf, int len)
{
	unsigned char *ptr;
	int txlen;
	int n = 0;
	
	do
	{
		txlen = len - n;
		txlen = (txlen < fifo->cache_block_size)? txlen : fifo->cache_block_size;
		ptr = serial_alloc(fifo);
		if( ptr )
		{
			memcpy(ptr, (unsigned char *)buf+n, txlen);
			if( serial_append(fifo, ptr, txlen) < 0)
			{
				break;
			}
			n += txlen;
		}
		else
		{
			SERIAL_ASSERT(RAW_SUCCESS ==raw_block_release(&fifo->cache, fifo->stream));
			raw_sleep(1);
		}
	}while(n < len);
	
	return n;
}

void serial_fifo_int_recv(serial_fifo_t *fifo, int (*fifo_read)(unsigned char *buf))
{
	unsigned char *ptr;
	unsigned char len = 0;
	
	ptr = serial_alloc(fifo);
	
	len = fifo_read(ptr);
	if( ptr) 
	{
		serial_append(fifo, ptr, len);
	}
}
