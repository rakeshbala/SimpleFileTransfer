/*************************************************
Rakesh Balasubramanian

September 14th, 2014
*************************************************/

#include "global.h"
#include "pa1_command_statistics.h"


#include <stdio.h>
#include <stdlib.h>
#include <string.h>


void command_statistics(client_list *theList, RUNNING_MODE runningMode){

	client_list *loopList = theList;	
	if (loopList==NULL)
	{
		printf("No connections exist.\n");
	}

	if (runningMode==kCLIENT_MODE)
	{
		printf("------------------------------- Statistics -------------------------------------\n");
		printf("%-28s%-14s%-11s%-16s%-11s\n","Hostname",
					"Total uploads",
					"Average ",
					"Total downloads",
					"Average ");
		printf("%-28s%-14s%-11s%-16s%-11s\n"," "," ","upload"," ","download");
		printf("%-28s%-14s%-11s%-16s%-11s\n"," "," ","speed(bps)"," ","speed(bps)");
		printf("--------------------------------------------------------------------------------\n");
			
		while(loopList != NULL){

			if (loopList->connection_id==1) //ignore server
			{
				loopList=loopList->cl_next;
				continue;
			}

			double avg_txrate = 0;
			if (loopList->upload_count!=0)
			{
				avg_txrate = loopList->sum_txrate/loopList->upload_count;
			}
			double avg_rxrate = 0;
			if (loopList->download_count != 0)
			{
				avg_rxrate = loopList->sum_dwrate/loopList->download_count;

			} 
			printf("%-28s%-14d%-11f%-16d%-11f\n",
				loopList->host_name,
				loopList->upload_count,
				avg_txrate,
				loopList->download_count,
				avg_rxrate);

			loopList= loopList->cl_next;

		}


	}else{

	}


}