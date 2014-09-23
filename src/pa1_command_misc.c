/*************************************************
Rakesh Balasubramanian

September 22nd 2014
*************************************************/

#include "global.h"
#include "pa1_command_misc.h"
#include <stdlib.h>
#include <stdio.h>



void commandList(client_list *theList){

	if (theList==NULL)
	{
		printf("No connections made.\n");
		return;
	}
	printf("                            Connections\n");
    printf("-------------------------------------------------------------------------\n");
	printf("%-5s%-35s%-20s%-8s\n","ID","Hostname","IP Address","Port No.");
    printf("-------------------------------------------------------------------------\n");

	client_list *loopList = theList;
	/******* Count the entries *********/
	// client_list *countList = *theList;
	// int connectCount=0;
	// while(countList!=NULL){
	// 	connectCount++;
	// 	if (connectCount>3)
	// 	{
	// 		fprintf(stderr, "More than three connections.\n");
	// 		return;
	// 	}
	// 	countList=countList->cl_next;
	// }
	while(loopList!=NULL){
		// if (loopList->connection_id==argc)
		// {
		// 	/* code */
		// }
		int portNum = (int)strtol(loopList->port,NULL,10);
		printf("%-5d%-35s%-20s%-8d\n",
			loopList->connection_id,
			loopList->host_name,
			loopList->ip_addr,
			portNum);
		loopList = loopList->cl_next;
	}

}

