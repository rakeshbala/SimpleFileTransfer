/**********************************************************
File name   : sft_client_list_util.c
Description : Methods to modify the core linked list of type
              client_list
@author     : Rakesh Balasubramanian
@created    : 19th Sep 2014
**********************************************************/

#include "sft_client_list_util.h"

#include "global.h"
#include "sft_listen_procs.h"
#include "sft_ui.h"
#include "sft_cmd_validate.h"
#include "sft_network_util.h"

#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>


client_list *removed_list; //global to add connections that were removed

/****************************************************************
Description  : Add an entry to the client_list and populate values
              (except port)
@arg         : theList address of the list pointer
@arg         : file_desc socket file descriptor
@arg         : host_name host name of the entry
@return      : void
Ref: http://www.learn-c.org/en/Linked_lists
*****************************************************************/
void add_to_client_list(client_list **theList, int file_desc, char *host_name, char *ip_addr)
{

    /******* Create vars for temp access and new entry *********/
    client_list *current = *theList;
    client_list *new_entry;

    if (current == NULL) //Create new list if NULL
    {
        current = calloc (1,sizeof(client_list));
        current->connection_id = 1;
        current->file_desc = file_desc;
        current->host_name = strdup(host_name);
        current->ip_addr = strdup(ip_addr);
        current->cl_next = NULL;
        current->upload_count = 0;
        current->download_count = 0;
        current->sum_dwrate = 0;
        current->sum_txrate = 0;
        current->port = NULL;
        *theList = current;

    }
    else //Append to existing list
    {

        int current_cid = current->connection_id;
        new_entry = calloc(1, sizeof(client_list));
        new_entry->connection_id = current_cid+1;
        new_entry->file_desc = file_desc;
        new_entry->host_name = strdup(host_name);
        new_entry->ip_addr = strdup(ip_addr);
        new_entry->port = NULL;

        new_entry->cl_next = *theList;
        *theList = new_entry;
    }

    /******* Dont print when adding to sip_list *********/
    if (!(*theList == sip_list) && !(*theList == removed_list))
    {
        printf("\n%s (%s): Trying to connect (fd %d)...\n",host_name,ip_addr,file_desc);
    }
}



/****************************************************************
Description  : Remove an entry from the client_list and also add
               to the removed list
@arg         : theList address of the list pointer
@arg         : file_desc socket file descriptor
@return      : int status
Ref: http://www.learn-c.org/en/Linked_lists
*****************************************************************/
int remove_from_client_list(client_list **theList, int file_desc){
    if (*theList==NULL )
    {
        fprintf(stderr, "\nAll connections closed \n");
        return;
    }
    
    int i = 0;
    client_list * current = *theList;
    client_list * temp_node = NULL;

    /******* Add to removed list for statistics *********/
    if (current->port != NULL)
    {
         add_to_client_list(&removed_list, file_desc, current->host_name,current->ip_addr);
         add_port_to_client(removed_list, file_desc, current->port);
         setTransferRates(&removed_list, file_desc, current->upload_count,
            current->sum_txrate, current->download_count, current->sum_dwrate);

    }
    


    if (!(current == sip_list))//ignore if sip_list
    {
        close(file_desc);
        FD_CLR(file_desc,&master);
        printf("\nClient  %s (%s) disconnected (fd %d)...\n",
            current->host_name,current->ip_addr,file_desc);
    }

    if (current->file_desc == file_desc) {
        client_list * next_node = NULL;
        next_node = current->cl_next;
        current->cl_next=NULL;
        freeLinkedList(&current);
        current = next_node;
        *theList=current;
        return;
    }

    while (current->cl_next->file_desc != file_desc) {
        if (current->cl_next == NULL) {
            return -1;
        }
        current = current->cl_next;
    }
    temp_node = current->cl_next;
    (*theList)->cl_next = temp_node->cl_next;
    temp_node->cl_next=NULL;
    freeLinkedList(&temp_node);
    temp_node = NULL;
}


/****************************************************************
Description  : Populate port of an entry in the client_list
@arg         : theList client_list which contains the entry
@arg         : file_desc socket file descriptor
@arg         : port_num port number
@return      : void
*****************************************************************/
void add_port_to_client(client_list *theList, int file_desc ,char *port_num)
{

    client_list *loopList = theList;
    while(loopList!=NULL){
        if (loopList->file_desc == file_desc)
        {
            // loopList->port = (char *)calloc(6, sizeof(char));
            loopList->port = strdup(port_num);
            if (!(loopList==sip_list) && !(loopList == removed_list))
            {
                printf("%s (%s : %s): Connected. (fd: %d)...\n",                
                        loopList->host_name,
                        loopList->ip_addr,
                        port_num,
                        file_desc);
            }
            return;
        }
        loopList= loopList->cl_next;

    }

    fprintf(stderr, "Couldn't add port\n");

}

/****************************************************************
Description  : Change connection id of a particular entry in list
@arg         : theList address of the list pointer
@arg         : file_desc socket file descriptor
@arg         : cid connection id being assigned
@return      : void
*****************************************************************/
void change_connect_id(client_list **theList, int file_desc ,int cid)
{

    client_list *loopList = *theList;
    while(loopList!=NULL){
        if (loopList->file_desc == file_desc)
        {
            loopList->connection_id = cid;
            return;
        }
        loopList= loopList->cl_next;
    }
}


/****************************************************************
Description  : Assign null to the port of a particular entry
@arg         : theList address of the list pointer
@arg         : file_desc socket file descriptor
@return      : void

*****************************************************************/
void change_port_null(client_list **theList, int file_desc)
{

    client_list *loopList = *theList;
    while(loopList!=NULL){
        if (loopList->file_desc == file_desc)
        {
            loopList->port = NULL;
            return;
        }
        loopList= loopList->cl_next;
    }
}

/****************************************************************
Description  : Set transfer rates associated with each entry
@arg         : theList address to list pointer
@arg         : file_desc socket file descriptor
@arg         : upload_count # of uploads
@arg         : sum_txrate sum of upload rates
@arg         : download_count # of downloads
@arg         : sum_dwrate sum of download reates
@return      : void
*****************************************************************/
void setTransferRates(client_list **theList, int file_desc, 
    int upload_count,double sum_txrate, int download_count, double sum_dwrate)
{
    client_list *loopList = *theList;
    while(loopList!=NULL){
        if (loopList->file_desc == file_desc)
        {
            loopList->upload_count = upload_count;
            loopList->download_count = download_count;
            loopList->sum_txrate = sum_txrate;
            loopList->sum_dwrate = sum_dwrate;
            return;
        }
        loopList= loopList->cl_next;
    }
}


/****************************************************************
Description  : Find an entry from list with file descriptor
@arg         : theList client_list pointer
@arg         : theList address of an entry pointer
@arg         : file_desc socket file descriptor
@return      : bool indicating if successful fetch
*****************************************************************/
bool get_list_entry(client_list *theList,client_list **theEntry, int file_desc){
    *theEntry = theList;
    while(theEntry!=NULL){
        if ((*theEntry)->file_desc == file_desc)
        {
            return true;
        }

        (*theEntry)= (*theEntry)->cl_next;
    }
    return false;
}


/****************************************************************
Description  : Iterate and print the list
@arg         : theList list to be printed
@return      : bool indicating if list is printable
*****************************************************************/
bool printClientList(client_list *theList)
{
    client_list *print_list;
    print_list = theList;
    if (print_list == NULL)
    {
        return false;
    }
    printf("\n");
    printf("--------------------------- Available peers -----------------------------\n");
    printf("%-5s%-35s%-20s%-8s\n","CID","Host name","IP Address","Port");
    printf("-------------------------------------------------------------------------\n");
    for (; print_list!= NULL; print_list = print_list->cl_next)
    {

        printf("%-5d%-35s%-20s%-8s\n",
            print_list->connection_id,
            print_list->host_name,
            print_list->ip_addr,
            print_list->port);
    }
    printf("\n");
    return true;
}

/****************************************************************
Description  : Free memory allocated to the linked list
@arg         : theList address of the list pointer
@return      : void
*****************************************************************/
void freeLinkedList(client_list **theList){
    client_list *tempNode;
    while(*theList){
        tempNode = *theList;
        *theList=(*theList)->cl_next;
        free(tempNode->ip_addr);
        tempNode->ip_addr=NULL;
        free(tempNode->host_name);
        tempNode->host_name = NULL;
        free(tempNode->port);
        tempNode->port = NULL;
        free(tempNode);
        tempNode = NULL;
    }
    *theList = NULL;
   // theList = NULL;
}


