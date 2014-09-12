#include <string.h>
#include "command.h"
#include "cli.h"
#include "cli_simple.h"

#include "raw_api.h"

/*
 * Run a command using the selected parser.
 *
 * @param cmd	Command to run
 * @param flag	Execution flags (CMD_FLAG_...)
 * @return 0 on success, or != 0 on error.
 */
int run_command(const char *cmd, int flag)
{
	/*
	 * cli_run_command can return 0 or 1 for success, so clean up
	 * its result.
	 */
	if (cli_simple_run_command(cmd, flag) == -1)
		return 1;

	return 0;
}

/*
 * Run a command using the selected parser, and check if it is repeatable.
 *
 * @param cmd	Command to run
 * @param flag	Execution flags (CMD_FLAG_...)
 * @return 0 (not repeatable) or 1 (repeatable) on success, -1 on error.
 */
int run_command_repeatable(const char *cmd, int flag)
{
	return cli_simple_run_command(cmd, flag);
}

int run_command_list(const char *cmd, int len, int flag)
{
	int need_buff = 1;
	char *buff = (char *)cmd;	/* cast away const */
	int rcode = 0;

	if (len == -1) {
		len = strlen(cmd);
		/* the built-in parser will change our string if it sees \n */
		need_buff = strchr(cmd, '\n') != NULL;
	}
	if (need_buff) {
		buff = __cli_io.malloc(len + 1);
		if (!buff)
			return 1;
		memcpy(buff, cmd, len);
		buff[len] = '\0';
	}

	/*
	 * This function will overwrite any \n it sees with a \0, which
	 * is why it can't work with a const char *. Here we are making
	 * using of internal knowledge of this function, to avoid always
	 * doing a malloc() which is actually required only in a case that
	 * is pretty rare.
	 */
	rcode = cli_simple_run_command_list(buff, flag);
	if (need_buff)
		__cli_io.free(buff);

	return rcode;
}

/****************************************************************************/

#if defined(CONFIG_CMD_RUN)
int do_run(cmd_tbl_t *cmdtp, int flag, int argc, char * const argv[])
{
	int i;

	if (argc < 2)
		return CMD_RET_USAGE;

	for (i = 1; i < argc; ++i) {
		char *arg;

		arg = getenv(argv[i]);
		if (arg == NULL) {
			__cli_io.printf("## Error: \"%s\" not defined\n", argv[i]);
			return 1;
		}

		if (run_command(arg, flag) != 0)
			return 1;
	}
	return 0;
}
#endif


void cli_loop(void)
{
	cli_simple_loop();
}

void cli_init(void)
{

}

