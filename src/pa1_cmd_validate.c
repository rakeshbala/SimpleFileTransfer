/**
* cmd_arg_validate
*
* @param  argc int
* @param  argv char **
* @return 1 SUCCESS
*/

#include <stdio.h>
#include <stdlib.h>
#include "pa1_cmd_validate.h"

#include <string.h>


CMD_Validation_Error cmd_arg_validate (int argc, char **argv){

	/******* Check if there are enough arguments *********/
	if (argc != 3) {
		fprintf(stderr, "usage: rbalasub_assignment1 [-s -c] <port>\n");
		return kNotEnoughArguments;
	}

	/******* Check if whether specified client/server mode *********/
	if (strcmp(argv[1],"-c")&&strcmp(argv[1],"-C")&&strcmp(argv[1],"-s")&&strcmp(argv[1],"-S")) 
	{
		/* code */
		fprintf(stderr, "Unknown mode. User either option -c or -s\n");
		return kUnknownMode;
	}

	/******* Check for valid port number *********/
	{
		/******* Check if number *********/
		int i;
		int portLength = strlen(argv[2]);
		for (i = 0; i < portLength ; ++i)
			{
				if(	!(isdigit	(argv[2][i]))){
					i = portLength+1;
					break;
				}
			}
		if (i>portLength)
		{
			fprintf(stderr, "Invalid port number. Enter only number.\n");
			return kInvalidPort;
		}
	}
	
	/******* Check if within the valid range *********/
	int portNumber = strtol(argv[2],NULL,10);
	if (portNumber<1024||portNumber>32767)
		{
			fprintf(stderr, "Invalid port number. Enter port number between 1024 and 32767.\n");
			return kInvalidPort;	
		}	
	
	return kSuccess;
}