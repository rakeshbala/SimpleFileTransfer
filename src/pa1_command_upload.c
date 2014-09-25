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

void command_upload(client_list *theList, int connection_id, char *path)
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
		//+size of string 'file'
		int final_length = length+num_digits_add+1+4;

		char *fileBytes = (char *)calloc(final_length,sizeof(char));
		char *tempBuf = calloc(length,sizeof(char));
		size_t len = fread(tempBuf, 1, length, fp);

		if (len<0)
		{
			perror("read");
			free(fileBytes);
			return;
		}
		sprintf(fileBytes,"%d file ",final_length);
		memcpy(fileBytes+(final_length-length+1), tempBuf,length);
		int i;
		send_all(destination->file_desc,fileBytes,final_length);
		free(fileBytes);
		fclose(fp);

	}else{
		perror("file open");
	}


}
