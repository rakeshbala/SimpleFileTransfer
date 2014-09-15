/*************************************************
Rakesh Balasubramanian

September 13th
*************************************************/

#include "global.h"
#include "pa1_server_procs.h"
#include "pa1_ui.h"

#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <stdlib.h>
#include <strings.h>
#include <stdio.h>

#define PROMPT_NAME "\n[PA1]$ "

int listening_port;


/******* Function declarations *********/
void *get_in_addr(struct sockaddr *sa);


int setupServer(RUNNING_MODE runningMode)
{

    /******* Make the socket *********/
    int server_socket = bindSocket(listening_port);

    if (server_socket<0)
    {
        fprintf(stderr, "Failed to create socket\n");
        return -1;
    }

    fd_set master;
    fd_set read_fds; //temp for select()
    int fd_max; //tracking maximum fd_set
    FD_SET(0,&master); // stdin for select()
    FD_SET(server_socket, &master);
    fd_max = server_socket;

    //keep listening
    while(true)
    {

        read_fds = master; //going to select();

        if (select(fd_max+1, &read_fds, NULL, NULL, NULL) == -1)
        {
            perror("select");
            return;
        }
        int loopIn;
        struct sockaddr_storage remoteaddr; // client address
        socklen_t addrlen;
        int new_fd;
        char remoteIP[INET6_ADDRSTRLEN];

        for (loopIn = 0; loopIn < fd_max; ++loopIn)
        {
            if (FD_ISSET(loopIn, &read_fds))
            {
                if (loopIn == server_socket)
                {
                    addrlen = sizeof remoteaddr;
                    new_fd = accept(server_socket,(struct sockaddr*)&remoteaddr, &addrlen);
                    if (new_fd == -1)
                    {
                        perror("accept");
                        return;
                    }
                    else
                    {
                        FD_SET(new_fd, &master);
                        if(new_fd>fd_max)
                        {
                            fd_max = new_fd;
                        }
                        printf("New connection from %s on %d \n",
                               inet_ntop(remoteaddr.ss_family,
                                         get_in_addr((struct sockaddr *)&remoteaddr),
                                         remoteIP,
                                         INET6_ADDRSTRLEN),
                               new_fd);
                    }
                }
                else if(loopIn == 0)
                {
                    exitOrHoldCursor(runningMode,server_socket);
                }
                else
                {
                    //handle data from client
                    printf("Data from client\n");
                }
            }
        }
    }

    return server_socket;
}










