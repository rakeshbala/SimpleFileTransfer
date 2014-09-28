/**********************************************************
File name   : pa1_command_statistics.h
Description : functions related to thed command 'STATISTICS'
@author     : Rakesh Balasubramanian
@created    : 26 Sep 2014
**********************************************************/


#include "global.h"

#ifndef PA1_COMMAND_STATISTICS_H_
#define PA1_COMMAND_STATISTICS_H_ 

void command_statistics(client_list *theList,RUNNING_MODE runningMode);
void printHeader();
void sendStatistics(client_list *theList, int file_desc);
void printStatistics(client_list *theList, char *stat_string, int file_desc);

#endif