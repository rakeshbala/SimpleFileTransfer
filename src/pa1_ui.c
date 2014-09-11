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
#include <curses.h>
#include <ncurses.h>
#include <string.h>
#include <stdlib.h>


/******* Function declarations *********/
void drawSkeleton(RUNNING_MODE runningMode);
void tearDown();
void exitOrHoldCursor(RUNNING_MODE runningMode);

/******* Function definitions *********/
void setupWindow(RUNNING_MODE runningMode){

	initscr();						/* start the curses mode */
	noecho();
	keypad(stdscr,TRUE);
	drawSkeleton(runningMode);
	exitOrHoldCursor(runningMode);
	
}


void drawSkeleton(RUNNING_MODE runningMode){

	/******* Draw header *********/ 
	char *title;
	int row,col;			
	switch (runningMode){
		case kCLIENT_MODE: title = "P2P File Transfer - Client";break;
		case kSERVER_MODE: title = "P2P File Transfer - Server";break;
		default: title = "P2P File Transfer - Unknown mode";
	}
	getmaxyx(stdscr,row,col);		/* get the number of rows and columns */
	mvprintw(1,(col-strlen(title))/2,"%s\n",title);
	hline(ACS_HLINE,col);

	/******* Draw footer *********/
	move(row-2,0);
	hline(ACS_HLINE,col);
	char * exitMessage = "Press Shift+x to exit";
	mvprintw(row-1,(col-strlen(exitMessage))/2,"%s\n",exitMessage);

	/******* Draw prompt *********/
	move(3,0);
	printw("> ");
}


void exitOrHoldCursor(RUNNING_MODE runningMode){
	
	static char commandString[200];
	
	static int y,x,row,col;	
	getyx(stdscr,y,x);
 	getmaxyx(stdscr,row,col);		/* get the number of rows and columns */

	/******* Reset Screen if at bottom*********/
	if (!(y<=row-3))
	{
		erase();
		drawSkeleton(runningMode);
	}

	/******* Handle key press *********/
	char c = getch();
	if (c=='X'){
		/******* Handle exit sequence *********/
		printw("\nPress Shift+x again to cancel. Press any other key to exit. ");
		char check = getch();
		if (check=='X')
		{	
			printw("Cancelled.\n> ");
			exitOrHoldCursor(runningMode);
		}else{
			printw("Exiting...");
			tearDown();
		}
	}else if(c=='\n'){
		/******* Handle return key press *********/
		addch(c);
		int commandIndex;
		bool isCommand = checkIfCommand(commandString,&commandIndex);
		if (isCommand)
		{
			/******* Command found. Now dispatch it. *********/
			commandDispatch(commandIndex,runningMode);

		}else if(strlen(commandString)>1){
			/******* Unknown command if more than 1 word *********/
			printw("%s : Unknown command\n",commandString);
		}
		commandString[0]= '\0';
		printw("> ");
		exitOrHoldCursor(runningMode);

	}else if(c==KEY_BACKSPACE||c==KEY_DC||c==127){
		/******* Handle backspace *********/
		if(x>2){
			/******* Remove from char buffer *********/
			commandString[strlen(commandString)-1]='\0';

			/******* Delete charachter *********/
			move(y,x-1);
			delch();
			delch();
		}
		
		exitOrHoldCursor(runningMode);
	}else{
		/******* Handle ordinary key press *********/
		addch(c);
		int len = strlen(commandString);
		commandString[len] = c;
		commandString[len+1]= '\0';
		exitOrHoldCursor(runningMode);
	}

}




/******* End curses mode *********/
void tearDown(){
	endwin();
}