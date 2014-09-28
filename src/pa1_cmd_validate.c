/**********************************************************
File name   : pa1_cmd_validate.c
Description : functions related to command line argument 
              validation
@author     : Rakesh Balasubramanian
@created    : 28 Sep 2014
**********************************************************/

#include <stdio.h>
#include <stdlib.h>
#include "pa1_cmd_validate.h"
#include "global.h"
#include <string.h>

char * listening_port;

CMD_Validation_Status cmd_arg_validate (int argc, char **argv)
{

    /******* Check if there are enough arguments *********/
    printf("Checking arguments...\n");
    if (argc != 3)
    {
        fprintf(stderr, ANSI_COLOR_RED "usage: rbalasub_assignment1 \
[s c] <port>" ANSI_COLOR_RESET "\n");
        return kNotEnoughArguments;
    }

    /******* Check if whether specified client/server mode *********/
    printf("Checking for running mode...\n");
    if (strcmp(argv[1],"c")&&strcmp(argv[1],"C")
            &&strcmp(argv[1],"s")&&strcmp(argv[1],"S"))
    {
        /* code */
        fprintf(stderr, ANSI_COLOR_RED "Unknown mode. User either \
option c or s" ANSI_COLOR_RESET"\n");
        return kUnknownMode;
    }

    /******* Check for valid port number *********/
    printf("Checking the port number...\n");

    CMD_Validation_Status port_check = checkPort(argv[2]);
    if (port_check==kSuccess)   
    {
        listening_port = argv[2];

    }

    return port_check;
}


CMD_Validation_Status checkPort (char * port)
{

    /******* Check if number *********/
    int i;
    int portLength = strlen(port);
    for (i = 0; i < portLength ; ++i)
    {
        if(	!(isdigit(port[i])))
        {
            i = portLength+1;
            break;
        }
    }
    if (i>portLength)
    {
        fprintf(stderr, ANSI_COLOR_RED "Invalid port number. Enter \
only number." ANSI_COLOR_RESET "\n");
        return kInvalidPort;
    }


    /******* Check if within the valid range *********/
    int portNumber = strtol(port,NULL,10);
    if (portNumber<1024||portNumber>32767)
    {
        fprintf(stderr, ANSI_COLOR_RED "Invalid port number. Enter port \
number between 1024 and 32767." ANSI_COLOR_RESET"\n");
        return kInvalidPort;
    }
    return kSuccess;

}

