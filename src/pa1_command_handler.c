// pa1_command_handler.c
/*************************************************
Rakesh Balasubramanian
September 9th, 2014
*************************************************/

#include "global.h"
#include "pa1_command_handler.h"

#include <string.h>

/******* Global constants *********/
const char *gbl_commandArray[]={"CREATOR\0","HELP\0",
"MYIP\0","MYPORT\0","REGISTER\0","CONNECT\0","LIST\0",
"TERMINATE\0","EXIT\0","UPLOAD\0","DOWNLOAD\0","STATISTICS\0"};

/******* Macros *********/
#define CMD_ARRAY_LEN 12

/******* Function declarations *********/
void commandHelp(RUNNING_MODE runningMode);

/******* Function definitions *********/

bool checkIfCommand(char commandString[200],int *index){


	/******* Length hard coded. Will have to change if no. of commands change *********/
	int i;
	for (i = 0; i < CMD_ARRAY_LEN; ++i)
	{
		if (strcmp(commandString,gbl_commandArray[i])==0)
		{
			*index = i;
			return true;
		}
	}
	return false;
}


void commandDispatch(int commandIndex, RUNNING_MODE runningMode){
	switch (commandIndex){
		/******* Handle CREATOR *********/
		case 0: {
			printw("Name	  : Rakesh Balasubramanian\nUBIT Name : rbalasub\n\
E-mail	  : rbalasub@buffalo.edu\n");
			printw("I have read and understood the course academic integrity \
policy located at\nhttp://www.cse.buffalo.edu/faculty/dimitrio/courses/cse4589_f14/\
index.html#integrity\n"); 
			break;
		}
		/******* Handle HELP *********/
		case 1:{
			commandHelp(runningMode);
		}
	}

}

/******* HELP command *********/
void commandHelp(RUNNING_MODE runningMode){
	/******* Array to print *********/
	// char *printArray[12] = (char **)malloc(12*100*sizeof(char));
	// int printIndex =0;

	char *clientPrintArray[12] = {
		"CREATOR - Prints Author information",
		"HELP - Prints help information",
		"MYIP - Display the IP address of this process",
		"MYPORT - DDisplay the port on which this process \
is listening for incoming connections.",
		"REGISTER - Registers the client with the server.\
\n  Usage: REGISTER <SERVER IP> <PORT NO>",
		"CONNECT - Connects to a peer.\nUsage: CONNECT <DE\
STINATION> <PORT NO>",
		"LIST - Display all connections",
		"TERMINATE - Terminate a connection using the connection id\
\n  <Usage: TERMINATE <CONNECTION ID>",
		"EXIT - Close all connections and exit the process",
		"UPLOAD - Uploads a file to a peer.\n  Usage: UPLOAD \
<CONNECTION ID> <FILE NAME>",
		"DOWNLOAD - Downloads a file from a peer.\n  Usage: DOWNLOAD \
<CONNECTION ID1> <FILE NAME1> <CONNECTION ID2> <FILE NAME2> ..",
		"STATISTICS - Display statistics"
	};
	int i;
	for (i = 0; i < CMD_ARRAY_LEN; ++i)
		{
				/******* Skip command indexes if server*********/
			if(runningMode == kSERVER_MODE){
				switch (i){
					case 4:
					case 5:
					case 9:
					case 10: {
						continue;
					}
					default: sleep(0);
				}
			}
			printw ("%s\n",clientPrintArray[i]);
		}
	

}



