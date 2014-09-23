/*************************************************
Rakesh Balasubramanian

September 21
*************************************************/
#include "global.h"
#include "pa1_client_connect.h"
#include "pa1_command_handler.h"

#include <stdlib.h>
#include <stdio.h>

char * my_ip_addr;

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
    	send_all(file_desc,"39 error Connect to server not allowed ",39);
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
    		send_all(file_desc,"48 error Destination not registered with server ",48);
    		fprintf(stderr, "Self not registered. Connection declined.\n");
    	}else{
    		send_all(file_desc,"36 error Not registered with server ",36);
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
        			alreadyExistFlag = 1;
        			break;
        		}
        	}
        }
        loopList = loopList->cl_next;
    }
    if (alreadyExistFlag)
    {
    	send_all(file_desc,"27 error Already connected ",27);
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
        	if (connectionCount>0)
        	{
        		moreThanThree=true;
        	}
        }
        loopList = loopList->cl_next;
    }
    if (moreThanThree)
    {
    	send_all(file_desc,"50 error Destination already connected to 3 peers ",50);
    	client_list *current;
    	get_list_entry(theList,&current,file_desc);
    	fprintf(stderr, "%s (%s) already connected. Duplicate connection declined.\n",
    		current->host_name,current->ip_addr);
    	return false;
    }

    return true;

}
