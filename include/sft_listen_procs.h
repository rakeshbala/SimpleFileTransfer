/**********************************************************
File name   : sft_listen_procs.h
Description : Main select multiplexing loop and dispatching
@author     : rab
@created    : 13th Sep 2014
**********************************************************/

#include "global.h"

#ifndef SFT_LISTEN_PROCS_H_
#define SFT_LISTEN_PROCS_H_ 


int listen_at_port(RUNNING_MODE runningMode, char *port);

#endif