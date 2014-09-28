/**********************************************************
File name   : pa1_command_upload.h
Description : functions related to command 'UPLOAD'
@author     : Rakesh Balasubramanian
@created    : 24 Sep 2014
**********************************************************/

#include "global.h"
#ifndef PA1_COMMAND_UPLOAD_H_
#define PA1_COMMAND_UPLOAD_H_ 

void command_upload(client_list *theList, int connection_id, char *path, TRANSFER_TYPE transferType);
bool writeToFile(char *data, char *fileName, int writeLength);

#endif

