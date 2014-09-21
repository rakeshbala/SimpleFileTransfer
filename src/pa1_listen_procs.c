/*************************************************
Rakesh Balasubramanian

September 13th
*************************************************/

#include "global.h"
#include "pa1_listen_procs.h"
#include "pa1_ui.h"
#include "pa1_cmd_validate.h"
#include "pa1_network_util.h"
#include "pa1_client_register.h"

#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

char * listening_port;


fd_set master; //master file desc set
int fd_max; //tracking maximum fd_set

/******* Function declarations *********/
char * create_list_string(client_list *theList);
void publish_list_to_client(char * ntw_string, int file_desc);


int listen_at_port(RUNNING_MODE runningMode, char * port)
{


    /******* Variable Declarations Begin *********/
    fd_set read_fds; //temp for select()
    int listening_socket ; //socket to listen
    int new_fd; //new socket when client connects

    struct sockaddr_storage remoteaddr; // client address
    socklen_t addrlen;  //length of address
    char remoteIP[INET6_ADDRSTRLEN];  //String for input address

    int yes = 1;


    char host_name [256]; //for incoming connections
    char service[20];

    int ii, jj; //loop variables
    int errorVar; //For reporting error with gai_strerror

    struct addrinfo hints, *receive_ai, *working_ai;
    client_list * theList = NULL;
    /******* Declarations End *********/


    FD_ZERO(&master);
    FD_ZERO(&read_fds);

    /******* Get i/p for socket creation *********/
    memset(&hints,0,sizeof hints);
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;

    if (checkPort(port)!=kSuccess)
    {
        return;
    }

    /******* Get addrinfo *********/
    if (errorVar = getaddrinfo(NULL, port, &hints, &receive_ai)!=0)
    {
        fprintf(stderr, "Get addr info: %s\n", gai_strerror(errorVar));
        exit(EXIT_FAILURE);
    }


    /******* Loop through addrinfos for binding a socket *********/
    for (working_ai = receive_ai; receive_ai!=NULL; receive_ai = receive_ai->ai_next)
    {

        /******* Create the socket if not continue the loop looking for other*********/
        listening_socket =  socket(working_ai->ai_family,working_ai->ai_socktype, 0);
        if (listening_socket<0)
        {
            continue;
        }

        /******* Make socket resuable *********/
        setsockopt(listening_socket, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int));

        /******* Bind else use another socket *********/
        if (bind(listening_socket, working_ai->ai_addr, working_ai->ai_addrlen)<0)
        {
            close(listening_socket);
            continue;
        }
        break;
    }

    /******* If something went wrong  *********/
    if (working_ai == NULL)
    {
        fprintf(stderr, "Failed to bind\n");
        exit(EXIT_FAILURE);
    }

    /******* Free the linked list *********/
    freeaddrinfo(receive_ai);


    /******* Listen for connections *********/
    if (listen(listening_socket,10)<0)
    {
        perror("listen");
        exit(EXIT_FAILURE);
    }


    FD_SET(0,&master); // stdin for select()
    FD_SET(listening_socket, &master); //for listening for connections
    fd_max = listening_socket;



    //keep listening
    for(;;)
    {

        read_fds = master; //going to select();
        if (select(fd_max+1, &read_fds, NULL, NULL, NULL) == -1)
        {
            perror("select");
            exit(EXIT_FAILURE);
        }
        for (ii = 0; ii <= fd_max; ii++)
        {
            if (FD_ISSET(ii, &read_fds))
            {
                /******* Take input from command line *********/
                if(ii == 0)
                {
                    exitOrHoldCursor(runningMode,listening_socket,theList);
                }
                /******* Check for incoming connections *********/
                else if (ii == listening_socket)
                {
                    addrlen = sizeof remoteaddr;
                    new_fd = accept(listening_socket,(struct sockaddr*)&remoteaddr, &addrlen);

                    if (new_fd == -1)
                    {
                        perror("accept");
                        return;
                    }
                    else
                    {
                        FD_SET(new_fd, &master); //Add the new socket to the fds
                        if(new_fd>fd_max)
                        {
                            fd_max = new_fd;
                        }
                        printf("\nNew connection from %s on %d...\n",
                           inet_ntop(remoteaddr.ss_family,
                             get_in_addr((struct sockaddr*)&remoteaddr),
                             remoteIP, INET6_ADDRSTRLEN),
                           new_fd);

                        //get host name

                        int name_status;
                        if (getnameinfo((struct sockaddr *)&remoteaddr,
                            sizeof remoteIP,
                            host_name,
                            sizeof host_name,service,sizeof service,0)<0)
                        {
                            fprintf(stderr, "Something wrong:%s\n", gai_strerror(name_status));
                        }
                        /******* Push it to the client list *********/
                        add_to_client_list(&theList, new_fd, host_name, remoteIP);

                    }
                }
                else// handle data from a client
                {
                    char recv_buf[256];  //256 bytes long recv_buffer
                    int nbytes;
                    if ((nbytes = recv(ii, recv_buf, sizeof recv_buf, 0)) <= 0)
                    {
                        // got error or connection closed by client
                        if (nbytes == 0)
                        {
                            // connection closed
                            remove_from_client_list(&theList,ii);
                            if (runningMode == kSERVER_MODE)
                            {
                                char *ntw_string = create_list_string(theList);

                            /******* From Beej's network programming guide *********/
                                int j;
                                for (j = 0; j <= fd_max; ++j)
                                {
                                    if (FD_ISSET(j,&master))
                                    {
                                        if (j!=0 && j!=listening_socket)
                                        {
                                            publish_list_to_client(ntw_string,j);
                                        }
                                    }
                                }
                            }
                            printf("\n");
                            printf(PROMPT_NAME);
                            fflush(stdout);
                            // printf("selectserver: socket %d hung up\n", ii);
                        }
                        else
                        {
                            perror("recv");
                        }
                        close(ii); // bye!
                        FD_CLR(ii, &master); // remove from master set
                    }
                    else
                    {
                        /******* Data received *********/

                        /******* Make sure all input is received *********/
                        char *arg; //var for storing tokenized input
                        char *recv_bufCopy;// = calloc(256, sizeof(char)); // needed since strtok modifies string
                        recv_bufCopy = strdup(recv_buf);
                        arg = strtok(recv_bufCopy," ");
                        int commandLen = strtol(arg,NULL,10);
                        if (commandLen-nbytes>0)
                        {
                            recv_all(ii,recv_buf,commandLen-nbytes);
                        }
                        recv_buf[commandLen+1]='\0';
                        free(recv_bufCopy);
                        /******* All inputs received *********/

                        /******* Tokenize and process *********/
                        int argc = 0;
                        char **argv = (char **)calloc(3, sizeof(char *));
                        // printf("\nRaw : %s %d\n", recv_buf,nbytes);
                        arg = strtok(recv_buf," ");
                        while(arg){
                            // argv[argc] = (char *)calloc(strlen(arg)+1, sizeof(char));
                            argv[argc] = strdup(arg);
                            argc++;
                            arg = strtok(NULL," ");
                        }

                        

                        /******* Handle the register command from client *********/
                        if (strcmp(argv[1],"register")==0)
                        {
                            add_port_to_client(theList,ii,argv[2]);
                            char *ntw_string = create_list_string(theList);

                            /******* From Beej's network programming guide *********/
                            int j;
                            for (j = 0; j <= fd_max; ++j)
                            {
                                if (FD_ISSET(j,&master))
                                {
                                    if (j!=0 && j!=listening_socket)
                                    {
                                        publish_list_to_client(ntw_string,j);
                                    }
                                }
                            }
                            printClientList(theList);
                            // printf("\n");
                            printf(PROMPT_NAME);
                        }

                        if (strcmp(argv[1],"server-ip-list")==0)
                        {
                            parseAndPrintSIPList(argv[2]);
                            printf("\n");
                            printf(PROMPT_NAME);
                        }

                        fflush(stdout);
                        int loopF;
                        for (loopF = 0; loopF < argc; ++loopF)
                        {
                            free(argv[loopF]);
                        }
                        free(argv);
                    }
                } 
            }
        }
    }
    freeLinkedList(theList);
    return listening_socket;

}



char * create_list_string(client_list *theList)
{
    char *ntw_string = (char *)calloc(1024, sizeof(char));
    strcat(ntw_string,"server-ip-list ");
    client_list* loopList = theList;
    while(loopList!=NULL){
        strcat(ntw_string,loopList->host_name);
        strcat(ntw_string,",");
        strcat(ntw_string,loopList->ip_addr);
        strcat(ntw_string,",");
        strcat(ntw_string,loopList->port);
        strcat(ntw_string,";"); //entry separator
        loopList = loopList->cl_next;
    }
    size_t length = strlen(ntw_string);
    char *lengthStr=calloc(4, sizeof(char));
    sprintf(lengthStr,"%zu",length+1);
    char *newString = (char *)calloc(strlen(ntw_string)+strlen(lengthStr), sizeof(char));
    sprintf(newString,"%s %s",lengthStr,ntw_string);
    free(ntw_string);
    free(lengthStr);
    return newString;
}

void publish_list_to_client(char *ntw_string,int file_desc){
    send_all(file_desc,ntw_string,strlen(ntw_string));
}