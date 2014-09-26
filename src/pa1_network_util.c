/*************************************************
Rakesh Balasubramanian

September 14th 2014
*************************************************/
#include "global.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <netdb.h>



// get sockaddr, IPv4 or IPv6:
/******* From Beej's network programming guide *********/
void *get_in_addr(struct sockaddr *sa)
{
    if (sa->sa_family == AF_INET)
    {
        return &(((struct sockaddr_in*)sa)->sin_addr);
    }

    return &(((struct sockaddr_in6*)sa)->sin6_addr);
}


/*************************************************
Idea from
http://stackoverflow.com/questions/13479760/c-
socket-recv-and-send-all-data
*************************************************/
bool send_all(int socket_fd, char *buffer,int buffer_len){

	char *ptr =  buffer;
	while(buffer_len>0){
		int bytesSent = send(socket_fd,ptr,buffer_len,0);
		if (bytesSent<0)
		{
			perror("send");
			return false;
		}
		ptr += bytesSent;
		buffer_len -= bytesSent;
	}
	return true;

}



bool recv_all(int socket_fd, char *buffer,int buffer_len, int nbytes){

	char *temp = (char *)calloc(buffer_len+1,sizeof(char));
	if (!temp)
	{
		perror("calloc");
		return false;
	}
	int tempLen= buffer_len;
	int copyHead = nbytes;
	while(tempLen>0){
		int bytesRecv = recv(socket_fd,temp,tempLen,0);
		if (bytesRecv<0)
		{
			perror("recv");
			free(temp);
			return false;
		}
		memcpy(buffer+copyHead,temp,bytesRecv);
		copyHead += bytesRecv;
		tempLen -= bytesRecv;
	}
	free(temp);
	return true;
}

int noOfDigits(int num){
    int digits = 0;
    while(num>0){
        digits++;
        num = num/10;
    }
    return digits;
}
