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

    client_list *loopList;
    if (runningMode==kSERVER_MODE)
    {
    	loopList=reverse(theList);	
    }else{
    	loopList=theList;
    }

	 
	while(loopList!=NULL){
		int portNum = (int)strtol(loopList->port,NULL,10);
		printf("%-5d%-35s%-20s%-8d\n",
			loopList->connection_id,
			loopList->host_name,
			loopList->ip_addr,
			portNum);
		loopList = loopList->cl_next;
	}

}


/*************************************************
From: http://stackoverflow.com/questions/1801549/
*************************************************/
client_list* reverse(client_list* root) {
  client_list* new_root = 0;
  while (root) {
    client_list* next = root->cl_next;
    root->cl_next = new_root;
    new_root = root;
    root = next;
  }
  return new_root;
}