/*************************************************
Rakesh Balasubramanian

September 14th 2014
*************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <netdb.h>

int bindSocket ( int port)
{
    int gen_socket = socket(PF_INET, SOCK_STREAM, 0);

    struct sockaddr_in self_addr;
    bzero(&self_addr,sizeof(self_addr));

    self_addr.sin_family = AF_INET;
    self_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    self_addr.sin_port = htons(port);

    // printf ("Port: %d",ntohs(self_addr.sin_port));

    if (bind(gen_socket, (struct sockaddr *)&self_addr,sizeof(self_addr))<0)
    {
        fprintf(stderr, "Not able to bind socket\n");
        return -1;
    }

    if (listen(gen_socket,10)<0)
    {
        fprintf(stderr, "Not able to listen\n");
        return -1;
    }

    return gen_socket;
}

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


