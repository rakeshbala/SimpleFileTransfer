/**********************************************************
File name   : sft_ui.h
Description : STDIN input and handling
@author     : Rakesh Balasubramanian
@created    : 8 Sep 2014
**********************************************************/
#include "global.h"
#ifndef SFT_UI_H_
#define SFT_UI_H_ 

void startApp(RUNNING_MODE runningMode, char *port);
void exitOrHoldCursor(RUNNING_MODE runningMode, int listening_socket, client_list **theList);

#endif