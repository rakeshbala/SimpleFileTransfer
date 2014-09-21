/*************************************************
Rakesh Balasubramanian
pa1_ui.h
September 8th, 2014
*************************************************/
#include "global.h"
#ifndef PA1_UI_H_
#define PA1_UI_H_ 

void startApp(RUNNING_MODE runningMode, char *port);
void exitOrHoldCursor(RUNNING_MODE runningMode, int listening_socket, client_list *theList);

#endif