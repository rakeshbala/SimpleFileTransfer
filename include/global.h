#ifndef GLOBAL_H_
#define GLOBAL_H_

#define HOSTNAME_LEN 128
#define MAX_PARALLEL_DOWNLOAD 3

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
