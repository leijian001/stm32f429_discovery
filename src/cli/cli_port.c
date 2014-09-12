#include <string.h>

#include "raw_api.h"

#include "cli.h"

#include "bsp.h"

void debug_putc(char c);
int debug_write(const void *buff, int len);
int debug_read(void *buff, int len);
int debug_cahce_empty(void);

static int cli_putc(char c)
{
	debug_putc(c);
	return c;
}
static int cli_puts(const char *str)
{
	debug_write(str, strlen(str));
	return 0;
}
static int cli_putnstr(const char *str, int n)
{
	debug_write(str, n);
	return 0;
}
static int cli_getc()
{
	char c;
	debug_read(&c, sizeof(char));
	return c;
}
int tstc(void)
{
	return ! debug_cahce_empty();
}

/******************************************************************************/

struct cli_io_t __cli_io =
{
	.putc = cli_putc,
	.puts = cli_puts,
	.putnstr = cli_putnstr,
	.getc = cli_getc,
	.printf = raw_printf,
	
	.malloc = port_malloc,
	.free   = port_free,
};

/******************************************************************************/

/* test if ctrl-c was pressed */
static int ctrlc_disabled = 0;	/* see disable_ctrl() */
static int ctrlc_was_pressed = 0;

int ctrlc(void)
{
	if ( !ctrlc_disabled ) {
		if (tstc()) {
			switch (__cli_io.getc()) {
			case 0x03:		/* ^C - Control C */
				ctrlc_was_pressed = 1;
				return 1;
			default:
				break;
			}
		}
	}
	return 0;
}
/* Reads user's confirmation.
   Returns 1 if user's input is "y", "Y", "yes" or "YES"
*/
int confirm_yesno(void)
{
	int i;
	char str_input[5];

	/* Flush input */
	while (tstc())
		__cli_io.getc();
	i = 0;
	while (i < sizeof(str_input)) {
		str_input[i] = __cli_io.getc();
		__cli_io.putc(str_input[i]);
		if (str_input[i] == '\r')
			break;
		i++;
	}
	__cli_io.putc('\n');
	if (strncmp(str_input, "y\r", 2) == 0 ||
	    strncmp(str_input, "Y\r", 2) == 0 ||
	    strncmp(str_input, "yes\r", 4) == 0 ||
	    strncmp(str_input, "YES\r", 4) == 0)
		return 1;
	return 0;
}
/* pass 1 to disable ctrlc() checking, 0 to enable.
 * returns previous state
 */
int disable_ctrlc(int disable)
{
	int prev = ctrlc_disabled;	/* save previous state */

	ctrlc_disabled = disable;
	return prev;
}

int had_ctrlc (void)
{
	return ctrlc_was_pressed;
}

void clear_ctrlc(void)
{
	ctrlc_was_pressed = 0;
}

