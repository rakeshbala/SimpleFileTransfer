/**********************************************************
File name   : pa1_cmd_validate.h
Description : functions related to command line argument 
			  validation
@author     : Rakesh Balasubramanian
@created    : 28 Sep 2014
**********************************************************/


#include "global.h"


#ifndef PA1_CMD_VALIDATE_H_
#define PA1_CMD_VALIDATE_H_

CMD_Validation_Status cmd_arg_validate (int argc, char **argv);
CMD_Validation_Status checkPort (char * port);

#endif
