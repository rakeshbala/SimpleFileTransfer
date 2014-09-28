/**********************************************************
File name   : pa1_ui.c
Description : STDIN input and handling
@author     : Rakesh Balasubramanian
@created    : 8 Sep 2014
**********************************************************/

/******* Include user header files *********/
#include "pa1_ui.h"
#include "global.h"
#include "pa1_command_handler.h"
#include "pa1_listen_procs.h"

/******* Include library files *********/
#include <stdio.h>
#include <string.h>
#include <stdlib.h>


/******* Macros *********/
#define ARG_ARRAY_LEN 10



/******* Function declarations *********/

/******* Function definitions *********/

/****************************************************************
Description  : Initialize and start the application processing
@arg         : runningMode client/server mode
@arg         : port listening port number of application
@return      : void
*****************************************************************/
void startApp(RUNNING_MODE runningMode, char * port)
{

    printf(ANSI_COLOR_GREEN "Starting application..." ANSI_COLOR_RESET);
    printf("\n");
    printf(PROMPT_NAME);


    /******* If server, start listening *********/
    int server_socket = listen_at_port(runningMode,port);
    if (server_socket<0)
    {
        fprintf(stderr, "Not able to setup server. Exiting\n");
        exit(EXIT_FAILURE);
    }
}


/****************************************************************
Description  : Do input processing and get input from STDIN and 
               dispatch input for processing
@arg         : runningMode client/server mode
@arg         : listening_socket socket file descriptor
@arg         : theList address of the list pointer
@return      : void
*****************************************************************/
void exitOrHoldCursor(RUNNING_MODE runningMode,int listening_socket, client_list **theList)
{
    /*************************************************
    Taken code snippet from :
    http://stackoverflow.com/questions/9278226/which-is
    -the-best-way-to-get-input-from-user-in-c
    *************************************************/
    // while(true){

    char commandString[512];
    if (fgets(commandString, sizeof(commandString), stdin))
    {

        /******* Handle enter key press  *********/
        commandString[strlen(commandString)-1]='\0'; //remove new line

        /******* From recitation slide *********/
        /******* Tokenize input *********/
        char *arg;
        int argc=0;
        char **argv = (char **)calloc(ARG_ARRAY_LEN, sizeof (char*));
        arg = strtok(commandString, " ");
        while(arg)
        {
            // argv[argc]=(char *)calloc(strlen(arg)+1,sizeof(char));
            argv[argc] = strdup(arg);
            argc++;
            if (argc>10)
            {
                fprintf(stderr, "Too many arguments\n");
                return;
            }
            arg = strtok(NULL," ");
        }

        /******* Send tokenized commands for processing *********/
        int status;
        if (!(argc==0))
        {
            status = processCommandArray(argc, argv,runningMode,theList);
            /******* Free the commands array *********/
            int ii =0;
            for (ii = 0; ii < argc; ++ii)
            {
                free(argv[ii]);
            }
            free (argv);
            if (status<0)
            {
                if (runningMode == kSERVER_MODE)
                {
                    close(listening_socket);
                }
                exit(EXIT_SUCCESS);
            }
            printf("\n");
        }
        printf(PROMPT_NAME);
    }
    else
    {
        fprintf(stderr, ANSI_COLOR_RED "Some error occured." ANSI_COLOR_RESET "\n");
    }
}
