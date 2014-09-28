/**********************************************************
File name   : pa1_ui.h
Description : STDIN input and handling
@author     : Rakesh Balasubramanian
@created    : 8 Sep 2014
**********************************************************/
#include "global.h"
#ifndef PA1_UI_H_
#define PA1_UI_H_ 

void startApp(RUNNING_MODE runningMode, char *port);
void exitOrHoldCursor(RUNNING_MODE runningMode, int listening_socket, client_list **theList);

#endif