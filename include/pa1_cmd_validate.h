/**
* pa1_cmd_validate.c
*
* Constants and functions related to command line argument validation
*/

typedef enum
{
	kSuccess,
	kNotEnoughArguments,
	kUnknownMode,
	kInvalidPort
} CMD_Validation_Error;

CMD_Validation_Error cmd_arg_validate (int argc, char **argv);