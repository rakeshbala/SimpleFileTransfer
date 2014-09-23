/*************************************************
Rakesh Balasubramanian

September 22nd 2014
*************************************************/

#include "global.h"
#include "pa1_command_misc.h"
#include <stdlib.h>
#include <stdio.h>


client_list* reverse(client_list* root);


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



/*************************************************
From: http://stackoverflow.com/questions/1801549/
*************************************************/
client_list* reverse(client_list* theList) {
	client_list *curNode = theList, *nxtNode;
	theList = NULL;

    // Until no more in list, insert current before theList and advance.
	while (curNode != NULL) {
        // Need to save next node since we're changing the current.
		nxtNode = curNode->cl_next;

        // Insert at start of new list.
		curNode->cl_next = theList;
		theList = curNode;

        // Advance to next.
		curNode = nxtNode;
	}
	return theList;
}