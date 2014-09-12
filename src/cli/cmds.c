#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <stm32f4xx.h>

#include "cli.h"
#include "command.h"

#include "raw_api.h"

#include "bsp.h"
#include "ff.h"
#include "ymodem.h"

extern int raw_printf(const char *format, ...);



static int do_help(struct cmd_tbl_s *cmdtp, int flag, int argc, char * const argv[])
{
	cmd_tbl_t *start = ll_entry_start(cmd_tbl_t, cmd);
	const int len = ll_entry_count(cmd_tbl_t, cmd);
	return _do_help(start, len, cmdtp, flag, argc, argv);
}

static int do_reboot(struct cmd_tbl_s *cmdtp, int flag, int argc, char * const argv[])
{
	NVIC_SystemReset();
	
	return 0;
}

static int do_stack(struct cmd_tbl_s *cmdtp, int flag, int argc, char * const argv[])
{
	LIST *iter, *head;
	RAW_TASK_OBJ *task_ptr;
	
	RAW_U32 free_stack;
	RAW_U32 inx = 0;
	RAW_SR_ALLOC();
	
	head = &raw_task_debug.task_head;
	iter = head->next;
	
	for(iter = head->next; iter != head; iter = iter->next) 
	{
		task_ptr = raw_list_entry(iter, RAW_TASK_OBJ, task_debug_list);
		
		RAW_CPU_DISABLE();
		free_stack = (unsigned int)( (PORT_STACK *)(task_ptr->task_stack) - task_ptr->task_stack_base );
		RAW_CPU_ENABLE();
		
		raw_printf("%04d name = %s\r\t\t\t\tstack_size = %4d\tfree_stack = %4d\n", 
					inx, task_ptr->task_name, task_ptr->stack_size, free_stack);
		
		inx++;
	}
	
	return 0;
}

/******************************************************************************/

#include "ymodem.h"
#define YMODEM_MAX_SIZE (10 * 1024)
const char *fatfs_err2str(FRESULT fret);
static int do_ry(struct cmd_tbl_s *cmdtp, int flag, int argc, char * const argv[])
{
	char filename[FILE_NAME_LENGTH];
	unsigned char *file_buf = port_malloc(YMODEM_MAX_SIZE);
	int file_size;
	unsigned int len;
	
	raw_printf("Ymodem receiving...\n");
	file_size = Ymodem_Receive (filename, file_buf, YMODEM_MAX_SIZE);
	if(file_size < 0)
	{
		raw_printf("receive error\n");
		goto do_ry_end;
	}
	raw_printf("receive OK\n");
	
	FIL fp;
	FRESULT fret;
	fret = f_open(&fp, filename, FA_OPEN_ALWAYS|FA_WRITE);
	if(FR_OK != fret)
	{
		raw_printf("open file error\n");
		raw_printf("%s\n", fatfs_err2str(fret));
		goto do_ry_end;	
	}
	fret = f_write(&fp, file_buf, file_size, &len);
	if(FR_OK != fret)
	{
		raw_printf("write file error\n");
		raw_printf("%s\n", fatfs_err2str(fret));
		f_close(&fp);
		goto do_ry_end;	
	}
	raw_printf("write file OK\n");
	
	f_close(&fp);
do_ry_end:
	port_free(file_buf);
	return 0;
}

static int do_cat(struct cmd_tbl_s *cmdtp, int flag, int argc, char * const argv[])
{
	if(2 != argc)
	{
		return -1;
	}
	
	FIL fp;
	FRESULT fret;
	char buf[32];
	unsigned int count;
	
	fret = f_open(&fp, argv[1], FA_READ);
	if(FR_OK != fret)
	{
		raw_printf("%s\n", fatfs_err2str(fret));
		return 0;
	}
	
	for(;;)
	{
		fret = f_read(&fp, buf, sizeof(buf)-1, &count);
		if(FR_OK != fret)
		{
			raw_printf("%s\n", fatfs_err2str(fret));
			break;
		}
		if(0 == count)
		{	// end
			break;
		}
		
		buf[count] = '\0';
		raw_printf("%s", buf);
	}
	
	raw_printf("\n");
	
	f_close(&fp);
	return 0;
}






/******************************************************************************/
static cmd_tbl_t __cmd_list[] = 
{
	U_BOOT_CMD_MKENT
	(
		help,   CONFIG_SYS_MAXARGS, 0/* 不重复 */,  do_help,
		"print command description/usage",
		"\n"
		"   - print brief description of all commands\n"
		"help command ...\n"
		"   - print detailed usage of 'command'\n"
	),
	U_BOOT_CMD_MKENT
	(
		reboot, 1, 0/* 不重复 */,  do_reboot,
		"reboot the system",
		"\n"
		"   - reboot the system\n"
	),
	U_BOOT_CMD_MKENT
	(
		stack, 1, 1,  do_stack,
		"print all task stack status",
		"\n"
		"   - print all task stack status\n"
	),
	U_BOOT_CMD_MKENT
	(
		ry, 1, 0,  do_ry,
		"Ymodem receive",
		"\n"
		"   - only support SOH"
		"\n"
	),
	U_BOOT_CMD_MKENT
	(
		cat, 2, 0,  do_cat,
		"print file",
		"\n"
		"   - only support SOH"
		"\n"
	),
};

int __ll_entry_count(void)
{
	return ARRAY_SIZE(__cmd_list);
}
const void *__ll_entry_start(void)
{
	return (void *)__cmd_list;
}
const void *__ll_entry_end(void)
{
	return (struct cmd_tbl_s *)__ll_entry_start + ARRAY_SIZE(__cmd_list);
}
