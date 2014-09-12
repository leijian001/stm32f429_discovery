#ifndef __SERIAL_FIFO_H__
#define __SERIAL_FIFO_H__

#include "raw_api.h"


typedef struct serial_fifo_s
{
	unsigned char *pos;
	unsigned char *stream;
	unsigned int count;
	
	RAW_QUEUE_SIZE fifo;
	MEM_POOL cache;
	
	RAW_MSG_SIZE *fifo_pool;
	unsigned char *cache_pool;
	
	int fifo_depth;
	int cache_block_size;
	
	RAW_MUTEX mutex;
}serial_fifo_t;

#define DEFINE_FIFO(name, num, block_size) \
	static RAW_MSG_SIZE  name##_fifo_pool[num]; \
	static unsigned char name##_cache_pool[(num) * (block_size)]; \
	static serial_fifo_t name##_fifo

#define FIFO_INIT(name, num, block_size) \
			serial_fifo_init(&(name##_fifo), \
							#name "_fifo",	\
							name##_fifo_pool, \
							num, \
							#name "_cache", \
							name##_cache_pool, \
							block_size, \
							#name "_mutex")
	
void serial_fifo_init(serial_fifo_t *fifo, 
						const char *fifo_pool_name,		// FIFO缓冲池名称
						RAW_MSG_SIZE *fifo_pool, 		// FIFO缓冲池地址
						unsigned int fifo_depth, 		// FIFO缓冲深度
						const char *cache_pool_name, 	// block内存池名称
						unsigned char *cache_pool, 		// block内存池地址
						unsigned int block_size, 		// block内存块大小
						const char *mutex_name);		// mutex名称
							
int serial_read_noblock(serial_fifo_t *fifo, RAW_TICK_TYPE wait_opt, void *buff, int len);
int serial_read(serial_fifo_t *fifo, void *buff, int len);
int serial_read_at_least_one(serial_fifo_t *fifo, RAW_TICK_TYPE wait_opt, void *buff, int len);
int serial_getchar(serial_fifo_t *fifo);
void serial_fifo_read_abort(serial_fifo_t *fifo);

int serial_fifo_append(serial_fifo_t *fifo, const void *buf, int len);

void serial_fifo_int_recv(serial_fifo_t *fifo, int (*fifo_read)(unsigned char *buf));
void serial_fifo_flush(serial_fifo_t *fifo);						

int serial_append(serial_fifo_t *fifo, void *buf, int len);
void *serial_alloc(serial_fifo_t *fifo);

#endif
