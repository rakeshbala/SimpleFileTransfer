/**********************************************************
File name   : pa1_network_util.h
Description : Generic network utility functions
@author     : Rakesh Balasubramanian
@created    : 14 Sep 2014
**********************************************************/

#include <sys/socket.h>
#include "global.h"

#ifndef PA1_NETWORK_UTIL_H_
#define PA1_NETWORK_UTIL_H_ 

void *get_in_addr(struct sockaddr *sa);
bool send_all(int socket_fd, char *buffer, int buffer_len);
bool recv_all(int socket_fd, char *buffer,int buffer_len, int buffer_offset);
int noOfDigits(int num);
bool recv_n_write(int socket_fd, char *buffer,int buffer_len, int nbytes, char *fileName);
#endif