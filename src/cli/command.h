#ifndef __COMMAND_H__
#define __COMMAND_H__

#include "cli_config.h"
#include "cli.h"

#ifndef NULL
#define NULL ((void *)0)
#endif


extern const void *__ll_entry_start(void);
extern const void *__ll_entry_end(void);
extern int __ll_entry_count(void);
#define ll_entry_declare(_type, _name, _list) const _type _cmd_##_name
#if 1
#define ll_entry_start(_type, _list) 	( (_type *)__ll_entry_start() )
#define ll_entry_end(_type, _list) 		( (_type *)__ll_entry_end() )
#else
#define ll_entry_start(_type, _list) 	( (_type *)0 )
#define ll_entry_end(_type, _list) 		( (_type *)0 )
#endif
#define ll_entry_count(_type, _list) 	( __ll_entry_count() )

/*
 * Monitor Command Table
 */
struct cmd_tbl_s {
	char	*name;			/* Command Name	   */
	int		maxargs;		/* maximum number of arguments	*/
	int		repeatable;		/* autorepeat allowed?		*/
	/* Implementation function	*/
	int		(*cmd)(struct cmd_tbl_s *, int, int, char * const []);
	char		*usage;		/* Usage message	(short)	*/
#ifdef	CONFIG_SYS_LONGHELP
	char		*help;		/* Help  message	(long)	*/
#endif
#ifdef CONFIG_AUTO_COMPLETE
	/* do auto completion on the arguments */
	int		(*complete)(int argc, char * const argv[], char last_char, int maxv, char *cmdv[]);
#endif
};
typedef struct cmd_tbl_s	cmd_tbl_t;

/*
 * Error codes that commands return to cmd_process(). We use the standard 0
 * and 1 for success and failure, but add one more case - failure with a
 * request to call cmd_usage(). But the cmd_process() function handles
 * CMD_RET_USAGE itself and after calling cmd_usage() it will return 1.
 * This is just a convenience for commands to avoid them having to call
 * cmd_usage() all over the place.
 */
enum command_ret_t {
	CMD_RET_SUCCESS,	/* 0 = Success */
	CMD_RET_FAILURE,	/* 1 = Failure */
	CMD_RET_USAGE = -1,	/* Failure, please report 'usage' error */
};

/*
 * Rather than doubling the size of the _ctype lookup table to hold a 'blank'
 * flag, just check for space or tab.
 */
#define isblank(c)	(c == ' ' || c == '\t')

/*
 * Command Flags:
 */
#define CMD_FLAG_REPEAT		0x0001	/* repeat last command		*/
#define CMD_FLAG_BOOTD		0x0002	/* command is from bootd	*/

static inline char *getenv(const char *argv)
{
	return 0;
}

#ifdef CONFIG_AUTO_COMPLETE
//extern int var_complete(int argc, char * const argv[], char last_char, int maxv, char *cmdv[]);
extern int cmd_auto_complete(const char *const prompt, char *buf, int *np, int *colp);
#endif

/**
 * Process a command with arguments. We look up the command and execute it
 * if valid. Otherwise we print a usage message.
 *
 * @param flag		Some flags normally 0 (see CMD_FLAG_.. above)
 * @param argc		Number of arguments (arg 0 must be the command text)
 * @param argv		Arguments
 * @param repeatable	This function sets this to 0 if the command is not
 *			repeatable. If the command is repeatable, the value
 *			is left unchanged.
 * @param ticks		If ticks is not null, this function set it to the
 *			number of ticks the command took to complete.
 * @return 0 if the command succeeded, 1 if it failed
 */
//int cmd_process(int flag, int argc, char * const argv[],
//			       int *repeatable, unsigned long *ticks);
enum command_ret_t cmd_process(int flag, int argc, char * const argv[],
			       int *repeatable, unsigned long *ticks);

/*
 * Use puts() instead of printf() to avoid printf buffer overflow
 * for long help messages
 */
int _do_help (cmd_tbl_t *cmd_start, int cmd_items, cmd_tbl_t * cmdtp, int
	      flag, int argc, char * const argv[]);

#ifdef CONFIG_AUTO_COMPLETE
# define _CMD_COMPLETE(x) x,
#else
# define _CMD_COMPLETE(x)
#endif
#ifdef CONFIG_SYS_LONGHELP
# define _CMD_HELP(x) x,
#else
# define _CMD_HELP(x)
#endif

#define U_BOOT_CMD_MKENT_COMPLETE(_name, _maxargs, _rep, _cmd, _usage, _help, _comp)			\
			{ #_name, _maxargs, _rep, _cmd, _usage,			\
			_CMD_HELP(_help) _CMD_COMPLETE(_comp) }

#define U_BOOT_CMD_MKENT(_name, _maxargs, _rep, _cmd, _usage, _help)	\
			U_BOOT_CMD_MKENT_COMPLETE(_name, _maxargs, _rep, _cmd,		\
					_usage, _help, NULL)

#define U_BOOT_CMD_COMPLETE(_name, _maxargs, _rep, _cmd, _usage, _help, _comp) \
			ll_entry_declare(cmd_tbl_t, _name, cmd) =			\
				U_BOOT_CMD_MKENT_COMPLETE(_name, _maxargs, _rep, _cmd, _usage, _help, _comp);

#define U_BOOT_CMD(_name, _maxargs, _rep, _cmd, _usage, _help)		\
			U_BOOT_CMD_COMPLETE(_name, _maxargs, _rep, _cmd, _usage, _help, NULL)

#include "raw_api.h"
static inline unsigned long get_timer(unsigned long base)
{
	return  (raw_tick_count * 10) - base;
}	
static inline unsigned long endtick(int tick)
{
	return raw_tick_count + tick;
}
static inline unsigned long get_ticks(void)
{
	return raw_tick_count;
}			
			
#endif
