/**********************************************************
File name   : sft_command_download.h
Description : functions related ot command "DOWNLOAD"
@author     : Rakesh Balasubramanian
@created    : 26 Sep 2014
**********************************************************/


#include "global.h"
#ifndef SFT_COMMAND_DOWNLOAD_H_
#define SFT_COMMAND_DOWNLOAD_H_ 

void command_download(client_list *theList, int connection_id, char *path);

#endif