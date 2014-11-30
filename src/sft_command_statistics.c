/**********************************************************
File name   : sft_command_statistics.c
Description : functions related to thed command 'STATISTICS'
@author     : Rakesh Balasubramanian
@created    : 26 Sep 2014
**********************************************************/

#include "global.h"
#include "sft_command_statistics.h"


#include <stdio.h>
#include <stdlib.h>
#include <string.h>


/****************************************************************
Description : Handle statistics command for client and server and
			  and display statistic information
@arg 		: theList  the list pointer
@arg 		: runningMode client/server mode
@return 	: void
*****************************************************************/
void command_statistics(client_list *theList, RUNNING_MODE runningMode){

	client_list *loopList = theList;	
	if (loopList==NULL)
	{
		printf("No connections exist\n");
		return;
	}
	if (runningMode==kCLIENT_MODE)
	{
		printf("------------------------------- Statistics -----------------------------------------\n");
		printf("%-32s%-14s%-11s%-16s%-11s\n","Hostname",
					"Total uploads",
					"Average ",
					"Total downloads",
					"Average ");
		printf("%-32s%-14s%-11s%-16s%-11s\n"," "," ","upload"," ","download");
		printf("%-32s%-14s%-11s%-16s%-11s\n"," "," ","speed(bps)"," ","speed(bps)");
		printf("------------------------------------------------------------------------------------\n");
		/******* Print current connections  *********/
		int client_count;
		while(loopList != NULL){
			client_count++;
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
			printf("%-32s%-14d%-11.0f%-16d%-11.0f\n",
				loopList->host_name,
				loopList->upload_count,
				avg_txrate,
				loopList->download_count,
				avg_rxrate);

			loopList= loopList->cl_next;

		}
		/******* Print old connections too *********/
		loopList = removed_list;
		while(loopList != NULL){
			client_count++;
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
			printf("%-32s%-14d%-11.0f%-16d%-11.0f\n",
				loopList->host_name,
				loopList->upload_count,
				avg_txrate,
				loopList->download_count,
				avg_rxrate);

			loopList= loopList->cl_next;

		}
		if (client_count<2)
		{
			printf("%-32s%-14s%-11s%-16s%-11s\n","-","-","-","-","-");
		}

	}else{
		printf("Querying clients...\n");
		while(loopList!=NULL){
			send_all(loopList->file_desc,"14 statistics ",14);
			loopList=loopList->cl_next;
		}
	}


}

/****************************************************************
Description : Send statistic from client to server
@arg 		: theList the list pointer
@arg 		: file_desc socket file descriptor
@return 	: void
*****************************************************************/
void sendStatistics(client_list *theList, int file_desc){


    char *ntw_string = (char *)calloc(1024, sizeof(char));
    if (!ntw_string)
    {
        perror("calloc");
        printf("Not able to initialize ntw_string\n");
        return;
    }
    strcat(ntw_string,"statistics ");
    
    /******* Creat string for existing connections *********/
    int client_count;
    client_list* loopList = theList;
    while(loopList!=NULL){
    	client_count++;
    	if (loopList->connection_id ==1)
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
        sprintf(ntw_string,"%s%s,%d,%.0f,%d,%.0f;",
            ntw_string,
            loopList->host_name,
            loopList->upload_count,
            avg_txrate,
            loopList->download_count,
            avg_rxrate);
        loopList = loopList->cl_next;
    }
    /******* Create string for old connections too *********/
    loopList= removed_list;
    while(loopList!=NULL){
    	client_count++;
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
        sprintf(ntw_string,"%s%s,%d,%.0f,%d,%.0f;",
            ntw_string,
            loopList->host_name,
            loopList->upload_count,
            avg_txrate,
            loopList->download_count,
            avg_rxrate);
        loopList = loopList->cl_next;
    }


    /******* Blanks if no connections other than server *********/
    if (client_count<2)
    {
    	sprintf(ntw_string,"%s-,-,-,-,-;",ntw_string);
    }
    size_t length = strlen(ntw_string);
    /******* Length edge case *********/
    int num_digits_temp = noOfDigits((int)length);
    int num_digits_add = noOfDigits((num_digits_temp+(int)length+1));
    if (num_digits_temp != num_digits_add)
    {
        num_digits_add++;
    }
    int final_length = length+num_digits_add+1;
    char *newString = (char *)calloc(final_length+1, sizeof(char));
    if (!newString)
    {
        perror("calloc");
        return;
    }
    sprintf(newString,"%d %s",final_length,ntw_string);
    send_all(file_desc,newString,final_length);
    free(newString);
    free(ntw_string);

}


/****************************************************************
Description : Print the statistics to stdout
@arg 		: theList  the list pointer
@arg 		: stat_string string to be parsed
@arg 		: file_desc socket file descriptor
@return 	: void
*****************************************************************/
void printStatistics(client_list *theList, char *stat_string, int file_desc)
{
	char *container;
    char *strTokInt;
    container = strtok_r(stat_string,";",&strTokInt);
    int dummyFd=2;

    while (container)
    {

        int argc=0;
        char **tokenArray = (char **)calloc(5, sizeof(char *));
        char * container2;
        char * strTokInt2;

        container2 = strtok_r(container,",",&strTokInt2);
        while (container2)
        {
            tokenArray[argc] = strdup(container2);
            argc++;
            container2 = strtok_r(NULL,",",&strTokInt2);
        }

        client_list *current;
        get_list_entry(theList,&current,file_desc);

        printf("%-32s%-32s%-8s%-14s%-10s%-11s\n", 
        	current->host_name,
        	tokenArray[0],
        	tokenArray[1],
        	tokenArray[2],
        	tokenArray[3],
        	tokenArray[4]);

        int i;
        for (i = 0; i < argc; ++i)
        {
            free(tokenArray[i]);
        }
        free(tokenArray);
        container = strtok_r(NULL,";",&strTokInt);
    }

}

/****************************************************************
Description  : Print header before statistics
@return 	 : void
*****************************************************************/
void printHeader(){

	printf("\n------------------------------------------ Statistics -------------------------------------------------\n");
	printf("%-32s%-32s%-8s%-14s%-10s%-11s\n","Hostname 1","Hostname 2",
		"Uploads",
		"Average ",
		"Downloads",
		"Average ");
	printf("%-32s%-32s%-8s%-14s%-10s%-11s\n"," "," "," ","upload"," ","download");
	printf("%-32s%-32s%-8s%-14s%-10s%-11s\n"," "," "," ","speed(bps)"," ","speed(bps)");
	printf("-------------------------------------------------------------------------------------------------------\n");

}
