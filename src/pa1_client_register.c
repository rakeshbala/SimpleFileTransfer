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

void commandRegister(char * destination, char *portStr)
{

    /******* Validate port *********/
    int portNumber;
    CMD_Validation_Status portStatus = checkPort(portStr,&portNumber);
    if (portStatus!=kSuccess)
    {
        fprintf(stderr, "Invalid Port\n");
        return;
    }

    /******* Get addrinfo of destination - Begin *********/
    struct addrinfo hints, *res, *dest_addr_info;
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

    for (dest_addr_info = res; dest_addr_info != NULL; dest_addr_info = dest_addr_info->ai_next)
    {
        if (dest_addr_info->ai_family==AF_INET) //break at the first found IPv4 address
        {
            /******* sockaddr in dest_addr_info *********/
            struct sockaddr_in *dest_sock_addr = (struct sockaddr_in *)dest_addr_info->ai_addr;
            void * temp_addr = &(dest_sock_addr->sin_addr);
            inet_ntop(AF_INET, temp_addr, ipstr,sizeof ipstr);
            printf("Testing");
            printf("IP : %s , Port %d\n", ipstr, ntohs(dest_sock_addr->sin_port));
            printf("Destination found\n");
            break;
        }
    }
    freeaddrinfo(res);

    /******* Get sockaddr of destination - End *********/

    /******* Connect to server *********/
    int connect_socket = socket( AF_INET,
                                 SOCK_STREAM,
                                 0);

    if(connect(connect_socket, dest_addr_info->ai_addr,res->ai_addrlen)<0)
    {
        perror("connect");
        return;
    }

    printf("Connected to server...\n");

    int numBytes = send(connect_socket,portStr,sizeof portStr,0);
    if (numBytes<0)
    {
        perror("Sending port failed.");
        return;
    }
    printf("Registration successful");

    exitOrHoldCursor(kCLIENT_MODE,connect_socket);


}
