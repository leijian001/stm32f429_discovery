#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <stm32f4xx.h>

#include "cli.h"
#include "command.h"

#include "raw_api.h"

#include "lua.h"
#include "lauxlib.h"
#include "lualib.h"
#include "lua_exlibs.h"
#include "lua_fatfs.h"

#include "bsp.h"
#include "ff.h"
#include "diskio.h"
#include "ymodem.h"

#pragma diag_suppress 870


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
	LIST *iter, *head, *temp;
	RAW_TASK_OBJ *task_ptr;
	
	RAW_U32 free_stack;
	RAW_U32 inx = 0;
	
	head = &raw_task_debug.task_head;
	iter = head->next;
	
	for(iter = head->next; iter != head; iter = temp, inx++) 
	{
		temp = iter->next;
		task_ptr = raw_list_entry(iter, RAW_TASK_OBJ, task_debug_list);
		
		raw_task_stack_check(task_ptr, &free_stack);
		
		raw_printf("%04d name = %s\r\t\t\t\tstack_size = %4d\tfree_stack = %4d\n", 
					inx, task_ptr->task_name, task_ptr->stack_size, free_stack);
	}
	
	return 0;
}

static int do_date(struct cmd_tbl_s *cmdtp, int flag, int argc, char * const argv[])
{
	if(1 != argc)
	{
		return -1;
	}
	
	char *w[] = {"日 \b", "一 \b", "二 \b", "三 \b", "四 \b", "五 \b", "六 \b"};
	
	raw_printf("%4d年 %02d月 %02d日 星期%s %2d:%02d:%02d\n",
		2014, 10, 1, w[3], 0, 0, 0);
	
	return 0;
}

/******************************************************************************/

#if 0
#define YMODEM_MAX_SIZE (10 * 1024)
const char *fatfs_err2str(FRESULT fret);
#endif
static int do_ry(struct cmd_tbl_s *cmdtp, int flag, int argc, char * const argv[])
{
#if 1
	ymodem_recv_to_fatfs();
	
	return 0;
#else
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
#endif
}

static int do_rm(struct cmd_tbl_s *cmdtp, int flag, int argc, char * const argv[])
{
	if(2 != argc)
	{
		return -1;
	}
	
	FRESULT fret;
	
	fret = f_unlink(argv[1]);
	if(FR_OK != fret)
	{
		raw_printf("%s\n", fatfs_err2str(fret));
	}
	
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

static int do_ls(struct cmd_tbl_s *cmdtp, int flag, int argc, char * const argv[])
{
	char *p;
	
	switch( argc )
	{
	case 1:
		p = "0:";
		break;
	case 2:
		p = argv[1];
		break;
	default:
		return -1;
	}
	
	scan_files(p);
	
	return 0;
}

lua_State *cmdL = 0;
static int do_lua(struct cmd_tbl_s *cmdtp, int flag, int argc, char * const argv[])
{
	if(2 != argc)
	{
		return -1;
	}
	
	RAW_ASSERT(NULL != cmdL);
	fatfs_dofile(cmdL, argv[1]);
	
	return 0;
}
static int do_lua_dostring(struct cmd_tbl_s *cmdtp, int flag, int argc, char * const argv[])
{
	if(2 != argc)
	{
		return -1;
	}

	RAW_ASSERT(NULL != cmdL);
	luaL_dostring(cmdL, argv[1]);
	
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
		"    - print brief description of all commands\n"
		"help command ...\n"
		"    - print detailed usage of 'command'\n"
	),
	U_BOOT_CMD_MKENT
	(
		reboot, 1, 0/* 不重复 */,  do_reboot,
		"reboot the system",
		"\n"
		"    - reboot the system\n"
	),
	U_BOOT_CMD_MKENT
	(
		stack, 1, 1,  do_stack,
		"print all task stack status",
		"\n"
		"    - print all task stack status\n"
	),
	U_BOOT_CMD_MKENT
	(
		ry, 1, 0,  do_ry,
		"receive file by Ymodem",
		"\n"
		"    - only support SOH\n"
	),
	U_BOOT_CMD_MKENT
	(
		ls, 2, 0,  do_ls,
		"scan file",
		"\n"
		"    - la path\n"
	),
	U_BOOT_CMD_MKENT
	(
		cat, 2, 0,  do_cat,
		"print file",
		"\n"
		"    - cat filename\n"
	),
	U_BOOT_CMD_MKENT
	(
		rm, 2, 0,  do_rm,
		"remove file",
		"\n"
		"    - rm filename\n"
	),
	U_BOOT_CMD_MKENT
	(
		date, 1, 1,  do_date,
		"print date and time",
		"\n"
		"    - print date and time\n"
	),
	U_BOOT_CMD_MKENT
	(
		lua, 2, 1,  do_lua,
		"run the lua script",
		"\n"
		"    - lua filename\n"
	),
	U_BOOT_CMD_MKENT
	(
		lua_dostring, 2, 1,  do_lua_dostring,
		"run the lua string",
		"\n"
		"    - lua string\n"
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
