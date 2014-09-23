/*************************************************
Rakesh Balasubramanian

September 19th 2014
*************************************************/
#include "global.h"

#ifndef PA1_CLIENT_LIST_UTIL_H_
#define PA1_CLIENT_LIST_UTIL_H_ 

void add_to_client_list(client_list ** theList,int file_desc,char *host_name, char *ip_addr);
int remove_from_client_list(client_list **theList, int file_desc);
bool printClientList(client_list *theList);
void freeLinkedList(client_list **theList);
int pop(client_list **theList,int file_desc);
void add_port_to_client(client_list *theList, int file_desc ,char *port_num);
bool get_list_entry(client_list *theList, client_list **theEntry,int file_desc);
void change_connect_id(client_list **theList, int file_desc ,int cid);
#endif
