/**********************************************************
File name   : pa1_listen_procs.c
Description : Main select multiplexing loop and dispatching
@author     : rab
@created    : 13th Sep 2014
**********************************************************/

#define _GNU_SOURCE 1

#include "global.h"
#include "pa1_listen_procs.h"
#include "pa1_ui.h"
#include "pa1_cmd_validate.h"
#include "pa1_network_util.h"
#include "pa1_client_register.h"
#include "pa1_client_connect.h"
#include "pa1_command_upload.h"

#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <sys/time.h>


char * listening_port;

fd_set master; //master file desc set
int fd_max; //tracking maximum fd_set
client_list *removed_list;

/******* Function declarations *********/
char * create_list_string(client_list *theList);
void publish_list_to_client(client_list *theList, int file_desc);



/****************************************************************
Description  : Select multiplexing over an infinte loop and handling 
               each network commands
@arg         : runningMode client/server mode
@arg         : port listening port
@return      : status to indicate success/error
Ref: http://beej.us/guide/bgnet/output/html/singlepage/bgnet.html#select
*****************************************************************/  
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
        freeaddrinfo(receive_ai);

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
    listen_socket = listening_socket;
    fd_max = listening_socket;



    //keep listening
    for(;;)
    {

        read_fds = master; //going to select();
        fflush(stdout);
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
                    exitOrHoldCursor(runningMode,listening_socket,&theList);
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

                        //get host name
                        int name_status;
                        if (getnameinfo((struct sockaddr *)&remoteaddr,
                            sizeof remoteIP,
                            host_name,
                            sizeof host_name,service,sizeof service,NI_NAMEREQD)<0)
                        {
                            fprintf(stderr, "Not able to find host name:%s\n", gai_strerror(name_status));
                        }

                        char *test;
                        inet_ntop(((struct sockaddr *)&remoteaddr)->sa_family,
                            get_in_addr((struct sockaddr*)&remoteaddr),
                             remoteIP, INET6_ADDRSTRLEN);
                        if (((struct sockaddr *)&remoteaddr)->sa_family==AF_INET6)
                        {
                            test = strndup (remoteIP+7,strlen(remoteIP)-7);
                        }else{
                            test = strdup(remoteIP);
                        }

                        /******* Push it to the client list *********/
                        add_to_client_list(&theList, new_fd, host_name, test);
                        free(test);



                    }
                }
                else// handle data from a client
                {
                    char *recv_buf = (char *)calloc(256,sizeof(char));  //256 bytes long recv_buffer
                    int nbytes;
                    struct timeval tv_start;
                    gettimeofday(&tv_start,NULL);
                    if ((nbytes = recv(ii, recv_buf, 256, 0)) <= 0)
                    {
                        // got error or connection closed by client
                        if (nbytes < 0)
                        {
                            perror("recv");
                        }
                        // connection closed
                        remove_from_client_list(&theList,ii);
                        close(ii); // bye! //Probably redundant
                        FD_CLR(ii, &master); // remove from master set

                        if (runningMode == kSERVER_MODE)
                        {
                            printClientList(theList); //print on self
                            publish_list_to_client(theList,listening_socket);
                        }
                        printf(PROMPT_NAME); //print the prompt

                    }
                    else
                    {



                        /******* Make sure all input is received *********/
                        char *arg; //var for storing tokenized input
                        char *recv_bufCopy; // needed since strtok modifies string
                        

                        /******* Receive first 50 or less charachters *********/
                        if (nbytes<50)
                        {
                            
                            recv_bufCopy = strdup(recv_buf);
                            arg = strtok(recv_bufCopy," ");
                            int receiveLength = 50;
                            int com_len = strtol(arg,NULL,10);
                            if (com_len<50)
                            {
                                receiveLength = com_len;
                            }else{

                            }
                            recv_buf = (char *)realloc(recv_buf,receiveLength+1);
                            if (receiveLength-nbytes>0)
                            {
                                recv_all(ii,recv_buf,receiveLength-nbytes,nbytes);
                            }
                            nbytes = receiveLength; //nbytes  = total received bytes
                            free(recv_bufCopy);
                        }


                        recv_bufCopy = strdup(recv_buf);
                        int f_count = 0;
                        char **f_array = (char **)calloc(50, sizeof(char *));
                        char *endString1;
                        arg = strtok(recv_bufCopy," ");
                        while(arg){
                            f_array[f_count] = strdup(arg);
                            arg = strtok(NULL," ");
                            f_count++;
                        }
                        free(recv_bufCopy);
                        /******* Handle special case of file write *********/
                        int commandLen = strtol(f_array[0],NULL,10);

                        if (strcmp(f_array[1],"file")==0)
                        {
                            
                            int bufLength =strtol(f_array[0],NULL,10);
                            int digitsLen = noOfDigits(bufLength);//Get no of digits of length
                            /******* DigitsLen+space+'file'+space+'fileName'+space *********/
                            int offset = digitsLen+1+4+1+strlen(f_array[2])+1;
                            bool write_status = recv_n_write(ii, recv_buf+offset,commandLen,nbytes,f_array[2]);
                            if (write_status)
                            {
                                struct timeval tv_end;
                                gettimeofday(&tv_end,NULL);

                                /******* Get receive rate *********/
                                long diff_usec = (tv_end.tv_sec*(long)1000000.0+tv_end.tv_usec) -
                                (tv_start.tv_sec*(long)1000000.0+tv_start.tv_usec);
                                int noOfBits = (commandLen-offset)*8;
                                double rxRate = (((float)noOfBits)/diff_usec)*1000000.0;

                                /******* Get my and destination host name *********/
                                char my_host_name[50];
                                gethostname(my_host_name,50);
                                client_list *host;
                                get_list_entry(theList, &host, ii);
                                printf("\nRx: %s -> %s, File size: %d Bytes, Time Taken: %f seconds, Rx Rate: %.2f bits/second\n",
                                    host->host_name,my_host_name,noOfBits/8,diff_usec/1000000.0, rxRate );

                                host->download_count++;
                                host->sum_dwrate += rxRate;

                            }    

                            printf(PROMPT_NAME);


                            free(recv_buf);
                            int loopF;
                            for (loopF = 0; loopF < f_count; ++loopF)
                            {
                                free(f_array[loopF]);
                            }
                            free(f_array);
                            continue;
                        }


                        /******* Receive rest of input *********/
                        recv_buf = (char *)realloc(recv_buf,commandLen+1);
                        if (commandLen-nbytes>0)
                        {
                            recv_all(ii,recv_buf,commandLen-nbytes,nbytes);
                        }
                        recv_buf[commandLen]='\0';
                        /******* All inputs received *********/

                        /******* Tokenize and process *********/
                        int argc = 0;
                        char **argv = (char **)calloc(50, sizeof(char *));
                        // printf("\nRaw : %s %d\n", recv_buf,nbytes);
                        char *endString;
                        recv_bufCopy = strdup(recv_buf);
                        arg = strtok_r(recv_bufCopy," ",&endString);
                        while(arg){
                            // argv[argc] = (char *)calloc(strlen(arg)+1, sizeof(char));
                            argv[argc] = strdup(arg);
                            arg = strtok_r(NULL," ",&endString);
                            argc++;
                            //break if parsing data string
                            if (argc>10)
                            {
                                break;
                            }
                        }
                        free(recv_bufCopy);


                        // printf("%s\n",recv_buf);
                        if (strcmp(argv[1],"register")==0)
                        {
                            if (runningMode==kCLIENT_MODE)
                            {
                                char *errorString = "22 error Not a server ";
                                send_all(ii,errorString,strlen(errorString));
                                remove_from_client_list(&theList,ii);
                            }
                            add_port_to_client(theList,ii,argv[2]);
                            publish_list_to_client(theList,listening_socket);
                            printClientList(theList);
                            printf(PROMPT_NAME);
                            // fflush(stdout);
                        }else if (strcmp(argv[1],"server-ip-list")==0)
                        {
                            parseAndPrintSIPList(argv[2]);
                            printf("\n");
                            printf(PROMPT_NAME);
                            // fflush(stdout);
                        }else if(strcmp(argv[1],"connect")==0){
                            bool connectStatus = validate_connect(theList,ii,argv[2]);
                            if (connectStatus)
                            {
                                add_port_to_client(theList,ii,argv[2]);
                            }else{
                                remove_from_client_list(&theList,ii);
                            }
                            printf(PROMPT_NAME);
                            // fflush(stdout);

                        }else if(strcmp(argv[1],"file")==0){
                            

                        }else if(strcmp(argv[1],"download")==0){
                            client_list *downloadEntry;
                            get_list_entry(theList,&downloadEntry,ii);
                            command_upload(theList,downloadEntry->connection_id,argv[2],kDOWN_FL);
                            printf(PROMPT_NAME);

                        }else if(strcmp(argv[1],"statistics")==0){
                            if (runningMode==kCLIENT_MODE)
                            {
                                sendStatistics(theList,ii);
                            }else{
                                /******* Get client count *********/
                                int connectCount = 0;
                                client_list *loopList=theList;
                                while(loopList!=NULL){
                                    connectCount++;
                                    loopList=loopList->cl_next;
                                }

                                /******* Print header only once *********/
                                static int printFlag = 0;
                                if (printFlag==0)
                                {
                                    printHeader();
                                }
                                printStatistics(theList, recv_buf+13, ii);
                                printFlag++;

                                /******* Reset printFlag if all clients finished printing *********/
                                if (printFlag==connectCount)
                                {
                                    printFlag = 0;
                                    printf(PROMPT_NAME);
                                }
                            }


                        }else if(strcmp(argv[1],"error")==0||strcmp(argv[1],"error-c")==0){

                            int offset = 9;
                            if (strcmp(argv[1],"error-c")==0)
                            {
                                offset = 11;
                                change_port_null(&theList, ii);
                            }

                            size_t length = (size_t)(strlen(recv_buf)-offset);
                            char *errorString= strndup(recv_buf+offset,length);
                            printf ("\nError: %s\n", errorString);
                            free(errorString);
                            if (strcmp(argv[1],"error")==0)
                            {
                                printf(PROMPT_NAME);
                            }
                        }else{
                            fprintf(stderr, "%s Invalid data\n",argv[1]);
                            printf(PROMPT_NAME);
                            // fflush(stdout);
                        }
                        // printf("\n");
                        // printf(PROMPT_NAME);
                        // fflush(stdout);
                        free(recv_buf);
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
    freeLinkedList(&theList);
    return listening_socket;

}


/****************************************************************
Description  : Create string to be sent to clients
@arg         : theList the list pointer
@return      : the created String
*****************************************************************/
char * create_list_string(client_list *theList)
{
    char *ntw_string = (char *)calloc(1024, sizeof(char));
    if (!ntw_string)
    {
        perror("calloc");
        printf("Not able to initialize ntw_string\n");
        return "";
    }
    strcat(ntw_string,"server-ip-list ");
    client_list* loopList = theList;
    while(loopList!=NULL){
        sprintf(ntw_string,"%s%s,%s,%s,%d;",
            ntw_string,
            loopList->host_name,
            loopList->ip_addr,
            loopList->port,
            loopList->connection_id);
        loopList = loopList->cl_next;
    }
    size_t length = strlen(ntw_string);
    /******* Length edge case *********/
    int num_digits_temp = noOfDigits((int)length);
    int num_digits_add = noOfDigits((num_digits_temp+(int)length+1));
    if (num_digits_temp != num_digits_add)
    {
        num_digits_add++;
    }
    int final_length = length+num_digits_add+1;
    // printf("Final length: %d\n", final_length);
    char *newString = (char *)calloc(final_length+1, sizeof(char));
    if (!newString)
    {
        perror("calloc");
        return "";
    }
    sprintf(newString,"%d %s",final_length,ntw_string);
    // printf("Server ip list: %s\n",newString);
    free(ntw_string);
    return newString;
}



/****************************************************************
Description  : Send the updated server ip list to all clients
@arg         : theList the list pointer
@arg         : listening_socket listening socket of server
@return      : void
Ref: http://beej.us/guide/bgnet/output/html/singlepage/bgnet.html#select
*****************************************************************/
void publish_list_to_client(client_list *theList, int listening_socket){

    char *ntw_string = create_list_string(theList);

    /******* From Beej's network programming guide *********/
    int j;
    for (j = 0; j <= fd_max; ++j)
    {
        if (FD_ISSET(j,&master))
        {
            if (j!=0 && j!=listening_socket)
            {
                send_all(j,ntw_string,strlen(ntw_string));
            }
        }
    }

    free(ntw_string);
}
