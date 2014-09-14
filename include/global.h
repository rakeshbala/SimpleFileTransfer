#ifndef GLOBAL_H_
#define GLOBAL_H_

#define HOSTNAME_LEN 128
#define MAX_PARALLEL_DOWNLOAD 3

/*************************************************
Taken from:
http://stackoverflow.com/questions/3219393/stdlib-
and-colored-output-in-c
*************************************************/
#define ANSI_COLOR_RED     "\x1b[31m"
#define ANSI_COLOR_GREEN   "\x1b[32m"
#define ANSI_COLOR_YELLOW  "\x1b[33m"
#define ANSI_COLOR_BLUE    "\x1b[34m"
#define ANSI_COLOR_MAGENTA "\x1b[35m"
#define ANSI_COLOR_CYAN    "\x1b[36m"
#define ANSI_COLOR_RESET   "\x1b[0m"

typedef enum
{
	kSuccess,
	kNotEnoughArguments,
	kUnknownMode,
	kInvalidPort
} CMD_Validation_Status;

typedef enum{
	kSERVER_MODE,
	kCLIENT_MODE
} RUNNING_MODE;

extern const char *gbl_commandArray[];
#endif
