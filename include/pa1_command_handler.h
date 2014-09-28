/**********************************************************
File name   : pa1_command_handler.h
Description : function for handling and dispatching typed in 
			  commands
@author     : Rakesh Balasubramanian
@created    : 9 Sep 2014
**********************************************************/

#include "global.h"


#ifndef PA1_COMMAND_HANDLER_H_
#define PA1_COMMAND_HANDLER_H_

int processCommandArray (int argc, char **argv, RUNNING_MODE runningMode, client_list **theList);

#endif
