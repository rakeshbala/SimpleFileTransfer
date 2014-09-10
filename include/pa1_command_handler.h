/*************************************************
Rakesh Balasubramanian
pa1_command_handler.h
September 9th, 2014
*************************************************/
#include "global.h"

#include <curses.h>

#ifndef PA1_COMMAND_HANDLER_H
#define PA1_COMMAND_HANDLER_H


bool checkIfCommand(char commandString[200],int *commandIndex);
void commandDispatch(int commandIndex);

#endif
