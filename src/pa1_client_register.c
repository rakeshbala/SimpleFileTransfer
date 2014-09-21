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


char * listening_port;

void commandRegister(char * destination, char *portStr, client_list *theList)
{

    /******* Validate port *********/
    CMD_Validation_Status portStatus = checkPort(portStr);
    if (portStatus!=kSuccess)
    {
        fprintf(stderr, "Invalid Port\n");  
        return;
    }

    /******* Get addrinfo of destination - Begin *********/
    struct addrinfo hints, *res, *dest_addr_info ;

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

    if(connect(connect_socket, dest_addr_info->ai_addr,dest_addr_info->ai_addrlen)<0)
    {
        perror("connect");
        return;
    }

    FD_SET(connect_socket,&master);
    if (connect_socket>fd_max)
    {
        fd_max = connect_socket;
    }

    char *commandStr = (char *)calloc(18, sizeof(char));
    strcat(commandStr,"16 register ");
    strcat(commandStr,listening_port);
    strcat(commandStr," ");
    int numBytes = send(connect_socket,commandStr, strlen(commandStr),0);
    if (numBytes<0)
    {
        perror("Sending port failed.");
        return;
    }
    printf("Connected to server...\n");
    free(commandStr);


    //get server name
    int name_status;
    char host_name [256];
    char service[20];
    if (getnameinfo(dest_addr_info->ai_addr,
                        dest_addr_info->ai_addrlen,
                        host_name,
                        sizeof host_name,service,
                        sizeof service,0 )<0)
    {
        fprintf(stderr, "Something wrong:%s\n", gai_strerror(name_status));
    }


    add_to_client_list(&theList, connect_socket, host_name,ipstr);
    add_port_to_client(theList,connect_socket,portStr);
}

void parseAndPrintSIPList(char *SIPlist){

    printf("\n                            Available peers                              \n");
    printf("-------------------------------------------------------------------------\n");
    printf("%-35s%-20s%-8s\n","Host name","IP Address","Port");
    printf("-------------------------------------------------------------------------\n");
    char *container;
    char *strTokInt;
    container = strtok_r(SIPlist,";",&strTokInt);

    while (container)
    {

        int argc=0;
        char **tokenArray = (char **)calloc(3, sizeof(char *));
        char * container2;
        char * strTokInt2;
        container2 = strtok_r(container,",",&strTokInt2);
        while (container2)
        {
            // tokenArray[argc] = calloc(strlen(container2)+1, sizeof(char));
            tokenArray[argc] = strdup(container2);
            argc++;
            container2 = strtok_r(NULL,",",&strTokInt2);
        }
        printf("%-35s%-20s%-8s\n",tokenArray[0],tokenArray[1],tokenArray[2]);
        int i;
        for (i = 0; i < argc; ++i)
        {
            free(tokenArray[i]);
        }
        free(tokenArray);
        container = strtok_r(NULL,";",&strTokInt);

    }
}
