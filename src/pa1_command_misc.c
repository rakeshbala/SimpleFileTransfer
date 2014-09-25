/*************************************************
Rakesh Balasubramanian

September 22nd 2014
*************************************************/

#include "global.h"
#include "pa1_command_misc.h"
#include <stdlib.h>
#include <stdio.h>




void commandList(client_list *theList,RUNNING_MODE runningMode){

	if (theList==NULL)
	{
		printf("No connections made.\n");
		return;
	}
	printf("                            Connections\n");
    printf("-------------------------------------------------------------------------\n");
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



void commandTerminate(client_list **theList, RUNNING_MODE runningMode, int connection_id)
{

	if (runningMode=kCLIENT_MODE&& connection_id == 1)
	{
		fprintf(stderr, "Terminate of server from client not allowed\n");
		return;
	}
	client_list *loopList = *theList;
	while(loopList!=NULL){
		if (loopList->connection_id == connection_id)
		{
			remove_from_client_list(theList,loopList->file_desc);
			return;
		}
	}

	fprintf(stderr, "No connections exist for the given connection id\n");

}


void commandExit(client_list **theList){

	client_list * loopList = *theList;
	while(loopList!=NULL){
		client_list *tempList = loopList;
		loopList= loopList->cl_next;
		remove_from_client_list(theList, tempList->file_desc);
	}
}