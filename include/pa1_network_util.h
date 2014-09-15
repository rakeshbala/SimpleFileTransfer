/*************************************************
Rakesh Balasubramanian

September 14th 2014
*************************************************/
#include <sys/socket.h>

#ifndef PA1_NETWORK_UTIL_H_
#define PA1_NETWORK_UTIL_H_ 

int bindSocket ( int port) ;
void *get_in_addr(struct sockaddr *sa);

#endif