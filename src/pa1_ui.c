/*************************************************
Rakesh Balasubramanian
PA1_UI.c -- UI Related code
September 8th 2014
*************************************************/

/******* Include user header files *********/
#include "pa1_ui.h"
#include "global.h"
#include "pa1_command_handler.h"

/******* Include library files *********/
#include <stdio.h>
#include <string.h>
#include <stdlib.h>


/******* Macros *********/
#define PROMPT_NAME "rbalasub_pa1"
#define ARG_ARRAY_LEN 10



/******* Function declarations *********/
void exitOrHoldCursor(RUNNING_MODE runningMode);

/******* Function definitions *********/
void startApp(RUNNING_MODE runningMode){

	printf(ANSI_COLOR_GREEN "Starting application..." ANSI_COLOR_RESET);
	printf("\n%s > ",PROMPT_NAME);
	exitOrHoldCursor(runningMode);
	
}



void exitOrHoldCursor(RUNNING_MODE runningMode){
	/*************************************************
	Taken code snippet from :
	http://stackoverflow.com/questions/9278226/which-is
	-the-best-way-to-get-input-from-user-in-c
	*************************************************/
	while(true){

		char commandString[512];
		if (fgets(commandString, sizeof(commandString), stdin)) {
			commandString[strlen(commandString)-1]='\0'; //remove new line

			/******* From recitation slide *********/
			/******* Tokenize input *********/
			char *arg;
			int argc=0;
			char **argv = (char **)malloc(ARG_ARRAY_LEN);
			arg = strtok(commandString, " ");
			while(arg){
				argv[argc]=(char *)malloc(strlen(arg)+1);
				strcpy(argv[argc],arg);
				argc++;
				if (argc>10){
					fprintf(stderr, "Too many arguments\n");
				}
				arg = strtok(NULL, " ");
			}
			
			/******* Send tokenized commands for processing *********/
			if (!(argc==0))
			{
				processCommandArray(argc, argv,runningMode);					
			}

			/******* Free the commands array *********/
			int ii =0;
			for (ii = 0; ii < argc; ++ii)
			{
				free(argv[ii]);
			}
			free (argv);
			
			printf("%s > ",PROMPT_NAME);



		}else{
			fprintf(stderr, ANSI_COLOR_RED "Some error occured." ANSI_COLOR_RESET "\n");
		}
	}
}