/**********************************************************
File name   : pa1_command_download.c
Description : functions related ot command "DOWNLOAD"
@author     : Rakesh Balasubramanian
@created    : 26 Sep 2014
**********************************************************/
#include "global.h"
#include "pa1_command_download.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>


/****************************************************************
Description : Download file from a destingation with connection id
@arg 		: theList the list pointer
@arg 		: connection_id connection_id from where download happens
@arg 		: path file path
@return 	: void
*****************************************************************/
void command_download(client_list *theList, int connection_id, char * path)
{
	if (connection_id==1)
	{
		fprintf(stderr, "Upload/Download from server not allowed \n" );
		return;
	}
	/******* Get the destination id *********/
	client_list *destination = theList;
	while(destination!=NULL){
		if (destination->connection_id == connection_id)
		{
			break;	
		}
		destination=destination->cl_next;
	}
	if (destination==NULL)
	{
		fprintf(stderr, "No connection with id %d\n", connection_id);
		return;
	}

	int pathLen = strlen(path);
	int sendLength = pathLen+13;
	char * sendString = (char *)calloc(sendLength+1,sizeof(char));
	sprintf(sendString,"%d download %s ",sendLength,path);
	send_all(destination->file_desc,sendString,sendLength);
	free(sendString);

	printf("Downloading file %s from %s (%s:%s) \n",
			path, 
			destination->host_name, 
			destination->ip_addr,
			destination->port);

}