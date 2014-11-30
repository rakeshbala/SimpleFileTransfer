/**********************************************************
File name   : sft_client_connect.h
Description : Validation of connect after calling connect
@author     : Rakesh Balasubramanian
@created    : 21st Sep 2014
**********************************************************/

#include "global.h"

#ifndef SFT_CLIENT_CONNECT_H_
#define SFT_CLIENT_CONNECT_H_ 

bool validate_connect(client_list *theList, int file_desc, char *port);


#endif