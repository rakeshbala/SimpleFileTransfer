/*************************************************
Rakesh Balasubramanian
pa1_command_handler.h
September 9th, 2014
*************************************************/
#include "global.h"

#include <curses.h>

#ifndef PA1_COMMAND_HANDLER_H
#define PA1_COMMAND_HANDLER_H

void processCommandArray (int argc, char **argv, RUNNING_MODE runningMode);
bool checkIfCommand(char *commandString,int *commandIndex);
void commandDispatch(int argc, char **argv, int commandIndex, RUNNING_MODE runningMode);

#endif
