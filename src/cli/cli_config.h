#ifndef __CLI_CONFIG_H__
#define __CLI_CONFIG_H__

#define CONFIG_SYS_PROMPT 	"raw-os# "	// 系统命令提示符

#define CONFIG_SYS_CBSIZE 	(80) 	// console I/O buffer size

#define CONFIG_SYS_MAXARGS 	(5) 	//
#define CONFIG_SYS_HELP_CMD_WIDTH 	"8"

#define CLI_POOL_SIZE 	1024 		// CLI 缓冲池大小

#define CONFIG_AUTO_COMPLETE 	 	// 自动补全
#define CONFIG_CMDLINE_EDITING 	 	// 使能历史命令
#define CONFIG_SYS_LONGHELP 	 	// 使能长帮助信息
#define CONFIG_CMD_RUN

#define SIGNLE_CR_LF 		0 		// 终端发送的是单个<CR>或<LF>置1; 如果同时发送<CR><LF>置0


#endif
