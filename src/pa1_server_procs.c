/*************************************************
Rakesh Balasubramanian

September 13th
*************************************************/

#include "global.h"
#include "pa1_server_procs.h"
#include "pa1_ui.h"
#include "pa1_cmd_validate.h"
#include "pa1_network_util.h"

#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>




int listening_port;

/******* Linked list declaration for holding all ips *********/
typedef struct server_ip_lst 
{
    int connection_id;
    char * host_name;
    char * ip_addr;
    char * port;

    struct server_ip_lst * cl_next;

} client_list;


/******* Function declarations *********/
void add_to_client_list(client_list ** server_ip_lst,char *host_name, char *ip_addr, char *port_num);
void printClientList(client_list *server_ip_lst);


int listen_at_port(char * port)
{


    /******* Variable Declarations Begin *********/
    fd_set master;
    fd_set read_fds; //temp for select()
    int fd_max; //tracking maximum fd_set
    int server_socket ; //socket to listen
    int new_fd; //new socket when client connects

    struct sockaddr_storage remoteaddr; // client address
    socklen_t addrlen;  //length of address
    char remoteIP[INET6_ADDRSTRLEN];  //String for input address

    char buf[256];  //256 bytes long buffer
    int nbytes;
    int yes = 1;

    int ii, jj; //loop variables
    int errorVar; //For reporting error with gai_strerror

    struct addrinfo hints, *receive_ai, *working_ai;

    client_list * server_ip_lst = NULL;
    /******* Declarations End *********/


    FD_ZERO(&master);
    FD_ZERO(&read_fds);

    /******* Get i/p for socket creation *********/
    memset(&hints,0,sizeof hints);
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;

    if (checkPort(port,NULL)!=kSuccess)
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
        server_socket =  socket(working_ai->ai_family,working_ai->ai_socktype, 0);
        if (server_socket<0)
        {
            continue;
        }

        /******* Make socket resuable *********/
        setsockopt(server_socket, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int));

        /******* Bind else use another socket *********/
        if (bind(server_socket, working_ai->ai_addr, working_ai->ai_addrlen)<0)
        {
            close(server_socket);
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
    if (listen(server_socket,10)<0)
    {
        perror("listen");
        exit(EXIT_FAILURE);
    }


    FD_SET(0,&master); // stdin for select()
    FD_SET(server_socket, &master); //for listening for connections
    fd_max = server_socket;



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
                    exitOrHoldCursor(kSERVER_MODE,server_socket);
                }
                /******* Check for incoming connections *********/
                else if (ii == server_socket)
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
                        FD_SET(new_fd, &master); //Add the new socket to the fds
                        if(new_fd>fd_max)
                        {
                            fd_max = new_fd;
                        }
                        printf("New connection from %s on %d \n",
                         inet_ntop(remoteaddr.ss_family,
                           get_in_addr((struct sockaddr*)&remoteaddr),
                           remoteIP, INET6_ADDRSTRLEN),
                         new_fd);

                        //get host name
                        char host_name [256];
                        char service[20];
                        int name_status;
                        if (getnameinfo((struct sockaddr *)&remoteaddr,
                            sizeof remoteIP,
                            host_name,
                            sizeof host_name,service,sizeof service,0 )<0)
                        {
                            fprintf(stderr, "Something wrong:%s\n", gai_strerror(name_status));
                        }

                        add_to_client_list(&server_ip_lst, host_name, remoteIP, "9999");

                    }
                }
                else
                {
                    // handle data from a client
                    if ((nbytes = recv(ii, buf, sizeof buf, 0)) <= 0)
                    {
                        // got error or connection closed by client
                        if (nbytes == 0)
                        {
                            // connection closed
                            printf("selectserver: socket %d hung up\n", ii);
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
                        printf ("Recieved: %s\n",buf);
                        fflush(stdout);

                    }
                }
            }
        }
    }
    return server_socket;

}


void add_to_client_list(client_list **server_ip_lst, char *host_name, char *ip_addr, char *port_num)
{

    client_list *current = *server_ip_lst;
    client_list *new_entry;

    if (current == NULL)
    {
        current = malloc (sizeof(client_list));
        current->connection_id = 1;
        current->host_name = host_name;
        current->ip_addr = ip_addr;
        current->port = port_num;
        current->cl_next = NULL;
        *server_ip_lst = current;

    }else{

        int current_cid = current->connection_id;
        
        new_entry = malloc(sizeof(client_list));
        new_entry->connection_id = current_cid++;
        new_entry->host_name = host_name;
        new_entry->ip_addr = ip_addr;
        new_entry->port = port_num;
        new_entry->cl_next = current;
        current = new_entry;
        *server_ip_lst = current;

    }

    printClientList(current);
}

void printClientList(client_list *server_ip_lst)
{
    client_list *print_list;
    for (print_list = server_ip_lst; print_list!= NULL; print_list = print_list->cl_next)
    {
        printf("%d %s %s %s\n",print_list->connection_id
          ,print_list->host_name
          ,print_list->ip_addr
          ,print_list->port);

    }

}




