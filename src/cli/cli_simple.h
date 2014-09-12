#ifndef __CLI_SIMPLE_H__
#define __CLI_SIMPLE_H__


void cli_simple_loop(void);
int cli_simple_run_command(const char *cmd, int flag);
int cli_simple_run_command_list(char *cmd, int flag);

#endif
