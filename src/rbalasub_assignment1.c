	/**
	 * @rbalasub_assignment1
	 * @author  Rakesh Balasubramanian <rbalasub@buffalo.edu>
	 * @version 1.0
	 *
	 * @section LICENSE
	 *
	 * This program is free software; you can redistribute it and/or
	 * modify it under the terms of the GNU General Public License as
	 * published by the Free Software Foundation; either version 2 of
	 * the License, or (at your option) any later version.
	 *
	 * This program is distributed in the hope that it will be useful, but
	 * WITHOUT ANY WARRANTY; without even the implied warranty of
	 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
	 * General Public License for more details at
	 * http://www.gnu.org/copyleft/gpl.html
	 *
	 * @section DESCRIPTION
	 *
	 * This contains the main function. Add further description here....
	 */
	#include <stdio.h>
	#include <stdlib.h>
	#include <ncurses.h>
	#include <string.h>
	#include "pa1_ui.h"
	 #include "pa1_cmd_validate.h"

	#include "../include/global.h"

	/**
	 * main function
	 *
	 * @param  argc Number of arguments
	 * @param  argv The argument list
	 * @return 0 EXIT_SUCCESS
	 */
	 int main(int argc, char **argv)
	 {

		/**** From Beej's network programming guide *********/

	 	//Check for command line arguments and validate
	 	CMD_Validation_Error status = cmd_arg_validate(argc,argv);
	 	if(status != kSuccess){
	 		return -1;
	 	}

	 	setupWindow(1);
	 	tearDown();
		return 0;
	}
