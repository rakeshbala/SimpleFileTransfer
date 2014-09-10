// pa1_command_handler.c
/*************************************************
Rakesh Balasubramanian
September 9th, 2014
*************************************************/

#include "global.h"
#include "pa1_command_handler.h"

#include <string.h>

/******* Function definitions *********/
const char *gbl_commandArray[]={"CREATOR","HELP",
"MYIP","MYPORT","REGISTER","CONNECT","LIST",
"TERMINATE","EXIT","UPLOAD","DOWNLOAD","STATISTICS"};



bool checkIfCommand(char commandString[200],int *index){


	/******* Length hard coded. Will have to change if no. of commands change *********/
	int i;
	for (i = 0; i < 12; ++i)
	{
		if (strcmp(commandString,gbl_commandArray[i])==0)
		{
			*index = i;
			return true;
		}
	}
	return false;
}


void commandDispatch(int commandIndex){
	switch (commandIndex){
		/******* Handle Creator *********/
		case 0: {
			printw("Name	  : Rakesh Balasubramanian\nUBIT Name : rbalasub\n\
E-mail	  : rbalasub@buffalo.edu\n");
			printw("I have read and understood the course academic integrity \
policy located at\nhttp://www.cse.buffalo.edu/faculty/dimitrio/courses/cse4589_f14/\
index.html#integrity\n"); 
			break;
		}
	}

}