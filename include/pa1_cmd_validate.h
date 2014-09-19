/**
* pa1_cmd_validate.c
*
* Constants and functions related to command line argument validation
*/

#include "global.h"


#ifndef PA1_CMD_VALIDATE_H_
#define PA1_CMD_VALIDATE_H_

CMD_Validation_Status cmd_arg_validate (int argc, char **argv);
CMD_Validation_Status checkPort (char * port);

#endif
