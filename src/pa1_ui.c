/*************************************************
Rakesh Balasubramanian
PA1_UI.c -- UI Related code
September 8th 2014
*************************************************/

/******* Include user header files *********/
#include "pa1_ui.h"
#include "global.h"
#include "pa1_command_handler.h"
#include "pa1_server_procs.h"

/******* Include library files *********/
#include <stdio.h>
#include <string.h>
#include <stdlib.h>


/******* Macros *********/
#define PROMPT_NAME "\n[PA1]$ "
#define ARG_ARRAY_LEN 10



/******* Function declarations *********/

/******* Function definitions *********/
void startApp(RUNNING_MODE runningMode)
{

    printf(ANSI_COLOR_GREEN "Starting application..." ANSI_COLOR_RESET);
    printf(PROMPT_NAME);
    fflush(stdout);

    /******* If server, start listening *********/
    if (runningMode == kSERVER_MODE)
    {
        int server_socket = setupServer();
        if (server_socket<0)
        {
            fprintf(stderr, "Not able to setup server. Exiting\n");
            exit(EXIT_FAILURE);
        }
        // exitOrHoldCursor(runningMode, server_socket);
    }
    else if(runningMode = kCLIENT_MODE)
    {
        exitOrHoldCursor(runningMode, 0);
    }


}



void exitOrHoldCursor(RUNNING_MODE runningMode,int server_socket)
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
        commandString[strlen(commandString)-1]='\0'; //remove new line

        /******* From recitation slide *********/
        /******* Tokenize input *********/
        char *arg;
        int argc=0;
        char **argv = (char **)malloc(ARG_ARRAY_LEN);
        arg = strtok(commandString, " ");
        while(arg)
        {
            argv[argc]=(char *)malloc(strlen(arg)+1);
            strcpy(argv[argc],arg);
            argc++;
            if (argc>10)
            {
                fprintf(stderr, "Too many arguments\n");
            }
            arg = strtok(NULL," ");
        }

        /******* Send tokenized commands for processing *********/
        int status;
        if (!(argc==0))
        {
            status = processCommandArray(argc, argv,runningMode);
        }

        /******* Free the commands array *********/
        int ii =0;
        for (ii = 0; ii < argc; ++ii)
        {
            free(argv[ii]);
        }
        free (argv);

        if (status<0)
        {
            if (runningMode = kSERVER_MODE)
            {
                close(server_socket);
            }
            exit(EXIT_SUCCESS);
        }

        printf(PROMPT_NAME);
        fflush(stdout);


    }
    else
    {
        fprintf(stderr, ANSI_COLOR_RED "Some error occured." ANSI_COLOR_RESET "\n");
    }
    // }
}
