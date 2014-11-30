/**********************************************************
File name   : sft_command_misc.h
Description : Functions related to "LIST", "TERMINATE" and
			  "EXIT"
@author     : Rakesh Balasubramanian
@created    : 28 Sep 2014
**********************************************************/


#include "global.h"

#ifndef SFT_COMMAND_MISC_H_
#define SFT_COMMAND_MISC_H_ 

void commandList(client_list * theList,RUNNING_MODE runningMode);
void commandTerminate(client_list **theList, RUNNING_MODE runningMode, int connection_id);
void commandExit(client_list **theList);

#endif