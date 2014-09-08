/*************************************************
Rakesh Balasubramanian
PA1_UI.c -- UI Related code
September 8th 2014
*************************************************/

#include "pa1_ui.h"
#include <stdio.h>
#include <ncurses.h>
#include <string.h>

int setupWindow(int main_window){

	/****, Print to the center of screen *********/
	int row,col;			
 	initscr();						/* start the curses mode */
 	getmaxyx(stdscr,row,col);		/* get the number of rows and columns */
 	char *title = "Falcon-Server";
	mvprintw(1,(col-strlen(title))/2,"%s\n",title);
	hline(ACS_HLINE,col);
	mvprintw(3,0,"\n");
	printw("Enter command: ");
	getch();
	
}



int tearDown(){
/**** TODO *********/

	printw("\nPress any key to exit.");
	getch();
	endwin();
}

