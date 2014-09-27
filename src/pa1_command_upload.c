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
#include <sys/time.h>

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


		off_t length = lseek(fileno(fp), 0, SEEK_END);
		rewind(fp);
		/******* String length edge case *********/
		int num_digits_temp = noOfDigits((int)length);
		int num_digits_add = noOfDigits((num_digits_temp+(int)length+1));
		if (num_digits_temp != num_digits_add)
		{
			num_digits_add++;
		}
		char *filename = basename(path);
		//+size of string = 'digits of size'+ ' '+'file'+' '+'filename'+' '

		/******* Send initial meta data *********/
		int metaLength = num_digits_add+1+4+1+strlen(filename)+1;
		int final_length = length+metaLength;
		char *metaData = (char *)calloc(metaLength+1, sizeof(char));
		sprintf(metaData,"%d file %s ",final_length,filename);
		send_all(destination->file_desc,metaData,metaLength);
		free(metaData);

		/******* Send file in chunks *********/
		int sendChunkSize= 100;		//Determines the chunk size

		/******* Mark start time *********/
		struct timeval tv_start;
		int status = gettimeofday(&tv_start,NULL);

		while(length>0){

			/******* Read in chunks *********/
			char *fileBytes = (char *)calloc(sendChunkSize,sizeof(char));
			size_t len = fread(fileBytes, 1, sendChunkSize, fp);
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
			/******* Send each chunk *********/
			send_all(destination->file_desc,fileBytes,len);
			length = length-len;
			free(fileBytes);
		}

		/******* Mark end time *********/
		struct timeval tv_end;
		status = gettimeofday(&tv_end,NULL);

		/******* Calculate total time *********/
		long diff_usec = (tv_end.tv_sec*(long)1000000.0+tv_end.tv_usec) -
			(tv_start.tv_sec*(long)1000000.0+tv_start.tv_usec);
		int noOfBits = (final_length-metaLength)*8; //length is lost
		double txRate = (((float)noOfBits)/diff_usec)*1000000;

		/******* Update the counters *********/
		destination->upload_count++;
		destination->sum_txrate += txRate;

		/******* Get my host name *********/
		char my_host_name[50];
		gethostname(my_host_name,50);
		printf("Tx: %s -> %s, File size: %d Bytes, Time Taken: %f seconds, Tx Rate: %.2f bits/second\n",
			my_host_name,destination->host_name,(final_length-metaLength),
			diff_usec/1000000.0, txRate );

		/******* Cleanup and print messages *********/
		fclose(fp);
		fflush(stdout);

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


/******* Writing to file in one shot. Doesn't affect the network speed *********/
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
