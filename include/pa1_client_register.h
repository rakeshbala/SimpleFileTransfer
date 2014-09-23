/*************************************************
Rakesh Balasubramanian

September 14th 2014
*************************************************/


#ifndef PA1_CLIENT_REGISTER_H_
#define PA1_CLIENT_REGISTER_H_


typedef enum {
	REGISTER_FL,
	CONNECT_FL
} connect_flag;

void command_connect(char *destingation, char *port, client_list **theList, connect_flag fl);
void parseAndPrintSIPList(char *SIPlist);



#endif
