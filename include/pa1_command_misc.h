/*************************************************
Rakesh Balasubramanian

September 22nd 2014
*************************************************/

#include "global.h"

#ifndef PA1_COMMAND_MISC_H_
#define PA1_COMMAND_MISC_H_ 

void commandList(client_list * theList,RUNNING_MODE runningMode);
void commandTerminate(client_list **theList, RUNNING_MODE runningMode, int connection_id);
void commandExit(client_list **theList);

#endif