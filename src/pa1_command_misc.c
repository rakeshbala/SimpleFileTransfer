/**********************************************************
File name   : pa1_command_misc.c
Description : Functions related to "LIST", "TERMINATE" and
			  "EXIT"
@author     : Rakesh Balasubramanian
@created    : 28 Sep 2014
**********************************************************/

#include "global.h"
#include "pa1_command_misc.h"
#include <stdlib.h>
#include <stdio.h>


int listen_socket;



/****************************************************************
Description : Handle list command and display information of all
			  connected peers
@arg 		: theList the list pointer
@arg 		: runningMode client/server mode
@return 	: void
*****************************************************************/
void commandList(client_list *theList,RUNNING_MODE runningMode){

	if (theList==NULL)
	{
		printf("No connections made.\n");
		return;
	}
    printf("--------------------------- Connections ---------------------------------\n");
	printf("%-5s%-35s%-20s%-8s\n","ID","Hostname","IP Address","Port No.");
    printf("-------------------------------------------------------------------------\n");

    client_list *loopList=theList;
   	int loopMax = loopList->connection_id;
   	int i;
   	for (i = 1; i <= loopMax; ++i)
   	{
   		while(loopList!=NULL){
   			if (loopList->connection_id == i)
   			{
   				int portNum = (int)strtol(loopList->port,NULL,10);
   				printf("%-5d%-35s%-20s%-8d\n",
   					loopList->connection_id,
   					loopList->host_name,
   					loopList->ip_addr,
   					portNum);
   				break;
   			}
   			loopList = loopList->cl_next;

   		}
   		loopList=theList;
   	}

}


/****************************************************************
Description : Handle terminate command and close connection to
			  a connected peer
@arg 		: theList address of the list pointer
@arg 		: runningMode client/server mode
@arg 		: connection_id connection id of client to terminate
@return 	: void
*****************************************************************/
void commandTerminate(client_list **theList, RUNNING_MODE runningMode, int connection_id)
{

	if (runningMode==kCLIENT_MODE && connection_id == 1)
	{
		fprintf(stderr, "Terminate of server from client not allowed\n");
		return;
	}
	client_list *loopList = *theList;
	while(loopList!=NULL){
		if (loopList->connection_id == connection_id)
		{
			remove_from_client_list(theList,loopList->file_desc);
			publish_list_to_client(*theList,listen_socket);
			return;
		}
		loopList=loopList->cl_next;
	}

	fprintf(stderr, "No connections exist for the given connection id\n");

}

/****************************************************************
Description : Exit the program after closing all connections
@arg 		: theList address of the list pointer
@return 	: void
*****************************************************************/
void commandExit(client_list **theList){

	client_list * loopList = *theList;
	while(loopList!=NULL){
		client_list *tempList = loopList;
		loopList= loopList->cl_next;
		remove_from_client_list(theList, tempList->file_desc);
	}
}