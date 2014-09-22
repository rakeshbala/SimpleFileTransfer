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
int fd_max;
fd_set master;
client_list *sip_list;
char * my_ip_addr;

void command_connect(char * destination, char *portStr, client_list *theList, connect_flag fl)
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
            // printf("Testing");
            // printf("IP : %s , Port %d\n", ipstr, ntohs(dest_sock_addr->sin_port));
            // printf("Destination found\n");
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
    if (fl==REGISTER_FL)
    {
        strcat(commandStr,"17 register ");
    }else if(fl==CONNECT_FL){
        strcat(commandStr,"11 connect ");
    }
    strcat(commandStr,listening_port);
    strcat(commandStr," ");
    int numBytes = send_all(connect_socket,commandStr,strlen(commandString));
    // send(connect_socket,commandStr, strlen(commandStr),0);
    if (numBytes<0)
    {
        perror("Send failed\n");
        return;
    }
    free(commandStr);

    //get host name
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

void parseAndPrintSIPList(char *SIPlist_str){

    // printf("Received string:\n %s\n", SIPlist_str);
    char *container;
    char *strTokInt;
    container = strtok_r(SIPlist_str,";",&strTokInt);
    int dummyFd=2;
    freeLinkedList(&sip_list);

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

        commandMyip();
        if (!(strcmp(tokenArray[1],my_ip_addr)==0 && strcmp(tokenArray[2],listening_port)==0))
        {
            add_to_client_list(&sip_list,dummyFd,tokenArray[0],tokenArray[1]);
            add_port_to_client(sip_list,dummyFd,tokenArray[2]);
            dummyFd++;
        }

        int i;
        for (i = 0; i < argc; ++i)
        {
            free(tokenArray[i]);
        }
        free(tokenArray);
        container = strtok_r(NULL,";",&strTokInt);
    }
    printClientList(sip_list);
}

