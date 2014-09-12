#ifndef __CLI_READLINE_H__
#define __CLI_READLINE_H__

#include "cli_config.h"
#include "cli.h"

extern char console_buffer[CONFIG_SYS_CBSIZE + 1];

int cli_readline(const char *const prompt);

int tstc(void);
int ctrlc(void);
int confirm_yesno(void);
int disable_ctrlc(int disable);
int had_ctrlc (void);
void clear_ctrlc(void);

#endif
