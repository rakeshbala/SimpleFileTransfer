/*************************************************
Rakesh Balasubramanian

September 14th 2014
*************************************************/


#include "global.h"
#include "pa1_client_register.h"
#include "pa1_network_util.h"
#include "pa1_cmd_validate.h"
#include "pa1_ui.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <netdb.h>


int listening_port;

void commandRegister(char * destination, char *portStr){

	/******* Validate port *********/
	int portNumber;
	CMD_Validation_Status portStatus = checkPort(portStr,&portNumber);
	if (portStatus!=kSuccess)
	{
		fprintf(stderr, "Invalid Port\n");
		return;
	}

	/******* Get addrinfo of destination - Begin *********/
	struct addrinfo hints, *res, *dest_sock_addr;
	int status;
	char ipstr [INET6_ADDRSTRLEN];
	
	memset (&hints, 0,sizeof hints);
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;

	if ((status = getaddrinfo(destination, portStr, &hints, &res)) != 0)
	{
		 fprintf(stderr, "getaddrinfo : %s\n", gai_strerror(status));
		 return ;
	}

	for (dest_sock_addr = res; dest_sock_addr != NULL; dest_sock_addr = dest_sock_addr->ai_next)
	{
		if (dest_sock_addr->ai_family==AF_INET) //break at the first found IPv4 address
		{
			/******* sockaddr in dest_sock_addr *********/
			printf("Destination found\n");
			break;
		}
	}
	freeaddrinfo(res);

	/******* Get sockaddr of destination - End *********/

	int connect_socket = socket( dest_sock_addr->ai_family,
								dest_sock_addr->ai_socktype,
								dest_sock_addr->ai_protocol);

	if(connect(connect_socket, dest_sock_addr->ai_addr,res->ai_addrlen)<0){
		perror("connect");
		return;
	}

	exitOrHoldCursor(kCLIENT_MODE,connect_socket);


}