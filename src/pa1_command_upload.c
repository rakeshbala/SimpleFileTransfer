/*************************************************
Rakesh Balasubramanian

September 24th
*************************************************/
#include "global.h"
#include "pa1_command_upload.h"
#include "pa1_network_util.h"

#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <libgen.h>

void command_upload(client_list *theList, int connection_id, char *path, TRANSFER_TYPE transferType)
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
	
	FILE *fp;
	if (fp = fopen(path, "rb"))
	{	

		/*************************************************
		http://stackoverflow.com/questions/238603/how-can-
		i-get-a-files-size-in-c
		*************************************************/
		off_t length = lseek(fileno(fp), 0, SEEK_END)+1;
		rewind(fp);
		/******* String length edge case *********/
		int num_digits_temp = noOfDigits((int)length);
		int num_digits_add = noOfDigits((num_digits_temp+(int)length+1));
		if (num_digits_temp != num_digits_add)
		{
			num_digits_add++;
		}
		//+size of string = 'digits of size'+ ' '+'file'+' '+length
		int final_length = length+num_digits_add+1+4+1;
		char *filename = basename(path);
		final_length = final_length + strlen(filename) + 1;


		char *fileBytes = (char *)calloc(final_length,sizeof(char));
		char *tempBuf = calloc(length,sizeof(char));

		size_t len = fread(tempBuf, 1, length, fp);
		if (len<0)
		{
			perror("read");
			if (transferType == kDOWN_FL)
			{
				send_all(destination->file_desc,"35 error Read error at destination ",35);
			}
			free(fileBytes);
			return;
		}


		sprintf(fileBytes,"%d file %s ",final_length,filename);
		memcpy(fileBytes+(final_length-length), tempBuf,length);
		int i;
		send_all(destination->file_desc,fileBytes,final_length);
		free(fileBytes);
		fclose(fp);
		if (transferType==kUP_FL)
		{
			printf("\nFile %s uploaded\n", filename);
		}else{
			printf("\nFile %s sent to %s (%s:%s) \n", 
				filename,
				destination->host_name,
				destination->ip_addr,
				destination->port);
		}

	}else{
		perror("file open");
		if (transferType == kDOWN_FL)
		{	
			int pathLen = strlen(path);
			int sendLength = pathLen+31;
			char * sendString = (char *)calloc(sendLength+1,sizeof(char));
			sprintf(sendString,"%d error File '%s' doesn't exist ",sendLength,path);
			send_all(destination->file_desc,sendString,sendLength);
			free(sendString);
		}
	}
}

bool writeToFile(char *data,char *fileName,int writeLength)
{
	FILE *fp;
	if (fp = fopen(fileName, "wb"))
	{
		int nbytes = fwrite(data,1,writeLength,fp);
		if (nbytes<0)
		{
			perror("write");
			fclose(fp);
			return false;
		}
	}else{
		perror("fopen");
		return false;
	}
	fclose(fp);
	return true;


}