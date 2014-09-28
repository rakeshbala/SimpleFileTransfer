/**********************************************************
File name   : pa1_network_util.c
Description : Generic network utility functions
@author     : Rakesh Balasubramanian
@created    : 14 Sep 2014
**********************************************************/
#include "global.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <netdb.h>



/****************************************************************
Description  : function pointer to get ipv4 or ipv6 depending on family
@arg 		 : sa the sockaddr structure for supplying family
@return 	 : void 
Ref: http://beej.us/guide/bgnet/output/html/singlepage/bgnet.html#select
*****************************************************************/
void *get_in_addr(struct sockaddr *sa)
{
    if (sa->sa_family == AF_INET)
    {
        return &(((struct sockaddr_in*)sa)->sin_addr);
    }

    return &(((struct sockaddr_in6*)sa)->sin6_addr);
}



/****************************************************************
Description : Send to socket all data irrespective of send buffer
			: restrictions
@arg 		: socket_fd socket file descriptor
@arg 		: buffer string/bytes to send
@arg 		: buffer_len length of bytes to send
@return 	: status indicating success/failure
Ref:http://stackoverflow.com/questions/13479760/c-socket-recv-and-send-all-data
*****************************************************************/	
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


/****************************************************************
Description : Receive from socket all data irrespective of recv buffer
			: restrictions
@arg 		: socket_fd socket file descriptor
@arg 		: buffer string/bytes already received
@arg 		: buffer_len length of bytes to receive
@arg 		: nbytes previously received bytes
@return 	: status indicating success/failure
*****************************************************************/	
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


/****************************************************************
Description : Write to file as and when receiving data
@arg 		: socket_fd socket file descriptor	
@arg 		: buffer data already received
@arg 		: arg3 data length to be received
@arg 		: fileName file name of the file to be written
@return 	: return
*****************************************************************/
bool recv_n_write(int socket_fd, char *buffer,int buffer_len, int nbytes, char *fileName){


	FILE *fp;
	if (fp = fopen(fileName, "ab")){

		int w_size = fwrite(buffer,1,strlen(buffer),fp);
		if (w_size<0)
		{
			perror("write");
			fclose(fp);
			return false;
		}
		
		int tempLen= buffer_len-nbytes;
		while(tempLen>0){
			char *temp = (char *)calloc(1000+1,sizeof(char));//allocate buffer
			if (!temp)
			{
				perror("calloc");
				return false;
			}
			int bytesRecv = recv(socket_fd,temp,1000,0);//receive
			if (bytesRecv<0)
			{
				perror("recv");
				free(temp);
				return false;
			}
			w_size = fwrite(temp,1,bytesRecv,fp);//write
			if (w_size<0)
			{
				perror("write");
				fclose(fp);
				return false;
			}
			tempLen -= bytesRecv; //update length
			free(temp);
		}
		printf("\nFile '%s' written to disk\n",fileName);
	}else{
		perror("fopen");
		return false;
	}
	fclose(fp);
	return true;
}



/****************************************************************
Description  : Get digits of a number
@arg 		 : num number whose digits needs to be calculated
@return 	 : number of digits
*****************************************************************/
int noOfDigits(int num){
    int digits = 0;
    while(num>0){
        digits++;
        num = num/10;
    }
    return digits;
}
