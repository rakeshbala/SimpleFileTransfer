/**********************************************************
File name   : pa1_client_connect.c
Description : Validation of connect after calling connect
@author     : Rakesh Balasubramanian
@created    : 21st Sep 2014
**********************************************************/
#include "global.h"
#include "pa1_client_connect.h"
#include "pa1_command_handler.h"

#include <stdlib.h>
#include <stdio.h>


char * my_ip_addr; //global to store my ip


/****************************************************************
Description : Validate connect 'after' being connected
@arg 		: theList client_list that holds all connections
@arg 		: file_desc socket file descriptor of incoming
@arg 		: port port string of incoming connection
@return 	: bool value indicating (un)successful validation
{9:References}
*****************************************************************/
bool validate_connect(client_list *theList, int file_desc, char *port){

	/******* Validate connect cases *********/

	client_list *loopList = theList;
	bool inSipFlag =false;
	bool alreadyExistFlag =false;
	bool isSelfFlag=false;
	bool isServerFlag=false;
	bool moreThanThree=false;
	char *ipAddr;
	//Get ip address
    while(loopList!=NULL){
        if (loopList->file_desc == file_desc) //Dont consider the last entry
        {
           	ipAddr= loopList->ip_addr;
            break;
        }
        loopList= loopList->cl_next;
    }

    /******* Validate if connecting to server *********/
    loopList=theList;
     while(loopList!=NULL){
        if (loopList->file_desc != file_desc) //Dont consider the last entry
        {
        	if (loopList->connection_id==1)
        	{
        		if (strcmp(loopList->ip_addr,ipAddr)==0)
        		{
        			if (strcmp(loopList->port,port)==0)
        			{
        				isServerFlag = true;
        				break;
        			}
        		}
        	}
        }
     	loopList= loopList->cl_next;    	
    }

    if (isServerFlag)
    {
    	send_all(file_desc,"41 error-c Connect to server not allowed ",41);
    	client_list *current;
    	get_list_entry(theList,&current,file_desc);
    	fprintf(stderr, "%s (%s) tried connect. Connection declined.\n",
    		current->host_name,current->ip_addr);
    	return false;
    }

    /******* Validate if in sip *********/
    loopList = sip_list;
    
    while(loopList!=NULL){
    	if (strcmp(loopList->ip_addr,ipAddr)==0)
    	{
    		if (strcmp(loopList->port,port)==0)
    		{
    			inSipFlag = true;
    			break;
    		}
    	}
        loopList= loopList->cl_next;
    }

    if (!inSipFlag) //checks if self registered too
    {
    	if (sip_list==NULL)
    	{
    		send_all(file_desc,"50 error-c Destination not registered with server ",50);
    		fprintf(stderr, "Self not registered. Connection declined.\n");
    	}else{
    		send_all(file_desc,"38 error-c Not registered with server ",38);
    		client_list *current;
    		get_list_entry(theList,&current,file_desc);
    		fprintf(stderr, "%s (%s) not registered. Connection declined.\n",
    			current->host_name,current->ip_addr);
    	}
    	return false;
    }


    /******* Validate if connection is duplicate *********/
    loopList=theList;
    while(loopList!=NULL){
        if (loopList->file_desc != file_desc) //Dont consider the last entry
        {
        	if (strcmp(loopList->ip_addr,ipAddr)==0)
        	{
        		if (strcmp(loopList->port,port)==0)
        		{
        			alreadyExistFlag = true;
        			break;
        		}
        	}
        }
        loopList = loopList->cl_next;
    }
    if (alreadyExistFlag)
    {
    	send_all(file_desc,"29 error-c Already connected ",29);
    	client_list *current;
    	get_list_entry(theList,&current,file_desc);
    	fprintf(stderr, "%s (%s) already connected. Duplicate connection declined.\n",
    		current->host_name,current->ip_addr);
    	return false;
    }

    /******* Validate if connection is to self *********/
    commandMyip(); //make sure my_ip_addr is valid
    if (strcmp(ipAddr,my_ip_addr)==0||
    	strcmp(ipAddr,"127.0.0.1")==0)
    {
    	if (strcmp(listening_port,port)==0)
    	{
    		isSelfFlag=1;
    	}
    }
    if (isSelfFlag)
    {
    	client_list *current;
    	get_list_entry(theList,&current,file_desc);
    	send_all(file_desc,"11 error-c ",11);
    	fprintf(stderr, "%s %s is self. Self connection declined.\n",
    		current->host_name,current->ip_addr);
    	return false;
    }

    loopList=theList;
    int connectionCount =0;
    while(loopList!=NULL){
        if (loopList->file_desc != file_desc) //Dont consider the last entry
        {
        	connectionCount++;
        	if (connectionCount>3)
        	{
        		moreThanThree=true;
        	}
        }
        loopList = loopList->cl_next;
    }
    if (moreThanThree)
    {
    	send_all(file_desc,"52 error-c Destination already connected to 3 peers ",52);
    	client_list *current;
    	get_list_entry(theList,&current,file_desc);
    	fprintf(stderr, "%s (%s) Maximum connections reached. New connection declined.\n",
    		current->host_name,current->ip_addr);
    	return false;
    }

    return true;

}
