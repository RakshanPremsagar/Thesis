/*
 ============================================================================
 Name        : open62541documentation.c
 Author      : Rakshan
 Version     :
 Copyright   : Your copyright notice
 Description : Hello World in C, Ansi-style
 ============================================================================
 */


#include "NamespaceID.h"
#include <signal.h>
#include <stdlib.h>
#include <time.h>
#include "open62541.h"
UA_Boolean running = true;
static void stopHandler(int sig) {
	running = false;
}



int main(void) {
	signal(SIGINT, stopHandler);
	signal(SIGTERM, stopHandler);
	//signal(SIGKILL,stopHandler);
	signal(SIGHUP, stopHandler);
	
	//create a timer using signal

	UA_Server *server = UA_Server_new();
	//printf("the value present in server is %d and %d", server, &server);
	UA_ServerConfig *config = UA_Server_getConfig(server);
	UA_ServerConfig_setDefault(config);
	//UA_ServerStatistics serv=  UA_Server_getStatistics(server);


	// This is an LDS server only. Set the application type to DISCOVERYSERVER.
	// NOTE: This will cause UaExpert to not show this instance in the server list.
	// See also: https://forum.unified-automation.com/topic1987.html
	config->applicationDescription.applicationType = UA_APPLICATIONTYPE_DISCOVERYSERVER;
	UA_String_clear(&config->applicationDescription.applicationUri);
	config->applicationDescription.applicationUri =
			UA_String_fromChars("urn:open62541.example.local_discovery_server");
	//callFunction(server);
	// Enable the mDNS announce and response functionality
	config->mdnsEnabled = true;
	config->mdnsConfig.mdnsServerName = UA_String_fromChars("LDS"); //-----uncomment this one

	// See http://www.opcfoundation.org/UA/schemas/1.03/ServerCapabilities.csv
	// For a LDS server, you should only indicate the LDS capability.
	// If this instance is an LDS and at the same time a normal OPC UA server, you also have to indicate
	// the additional capabilities.
	// NOTE: UaExpert does not show LDS-only servers in the list.
	// See also: https://forum.unified-automation.com/topic1987.html

	// E.g. here we only set LDS, and you will not see it in UaExpert
	config->mdnsConfig.serverCapabilitiesSize = 2;
	UA_String *caps = (UA_String *) UA_Array_new(2, &UA_TYPES[UA_TYPES_STRING]);
    	//config->mdnsConfig.mdnsServerName = UA_String_fromChars("LDSME");

	caps[0]= UA_String_fromChars("LDSME-SERVERSIDE");
	caps[1]= UA_String_fromChars("Available");

	config->mdnsConfig.serverCapabilities = caps;



	/* timeout in seconds when to automatically remove a registered server from
	 * the list, if it doesn't re-register within the given time frame. A value
	 * of 0 disables automatic removal. Default is 60 Minutes (60*60). Must be
	 * bigger than 10 seconds, because cleanup is only triggered approximately
	 * every 10 seconds. The server will still be removed depending on the
	 * state of the semaphore file. */


	//-----------------------------------------------------------------------
	//UA_StatusCode retval = UA_Server_run(server, &running);
	/*UA_StatusCode retval = UA_Server_run_startup(server);
	if(retval!= UA_STATUSCODE_GOOD){
		UA_Server_delete(server);
		return retval;
	}
	int timestamp = time(0) + 1;
	while(running== true){
			//caps[0]= UA_String_fromChars("NOT OK");
			//caps[1]= UA_String_fromChars("OK");
			//config->mdnsConfig.serverCapabilities = caps;
	//	caps[1]= UA_String_fromChars("Busy");
			UA_Server_run_iterate(server, true);

		if(time(0)>timestamp){
			timestamp= time(0) + 3;
			int count= getCountValue();//check if the session has established
			//int Tcount=(int)serv.ss.sessionTimeoutCount;
			//double num;
			//num= static_cast<double>count;
			printf("Num: %d\n",count);

			//printf("Tcount:%d\n",Tcount);
			//caps[1]= UA_String_fromChars("Available");
			if(count==1){
				caps[1]= UA_String_fromChars("Busy");
				printf("The server status is: %.*s\n", caps[1]);
			}
			else{
				printf("The server status is: %.*s\n", caps[1]);
			}
		    config->mdnsConfig.serverCapabilities = caps;
	}
	}*/
	//------------------------------------------------------------------------------------------



	//-- Call func, declare the global variable of mdns_daemon in open6265.c, inside that function, copy server.>disconver mang.daemon to that struct. Finally in remove record function, copy this global struct to daemon againt after free.
	UA_StatusCode retval = UA_Server_run(server, &running);

    UA_Server_delete(server);
	return retval == UA_STATUSCODE_GOOD ? EXIT_SUCCESS : EXIT_FAILURE;

}



































/*#include "open62541.h"
#include "NamespaceID.h"
#include <signal.h>
#include <stdlib.h>
#include <time.h>
UA_Boolean running = true;
static void stopHandler(int sig) {
	running = false;
}

//const UA_String x = ""; //define constant int
			   int *ptr;
			   printf("x = %d\n", x);
			   ptr = &x; //ptr points the variable x
			   *ptr = 15; //Updating through pointer
			   printf("x = %d\n", x);
//UA_String *caps = &x;
UA_String *caps;
int main(void) {
	signal(SIGINT, stopHandler);
	signal(SIGTERM, stopHandler);

	UA_Server *server = UA_Server_new();

	UA_ServerConfig *config = UA_Server_getConfig(server);
	UA_ServerConfig_setDefault(config);
	//UA_ServerStatistics serv=  UA_Server_getStatistics(server);

	// This is an LDS server only. Set the application type to DISCOVERYSERVER.
	// NOTE: This will cause UaExpert to not show this instance in the server list.
	// See also: https://forum.unified-automation.com/topic1987.html
	config->applicationDescription.applicationType = UA_APPLICATIONTYPE_DISCOVERYSERVER;
	UA_String_clear(&config->applicationDescription.applicationUri);
	config->applicationDescription.applicationUri =
			UA_String_fromChars("urn:open62541.example.local_discovery_server");

	// Enable the mDNS announce and response functionality
	config->mdnsEnabled = true;
	config->mdnsConfig.mdnsServerName = UA_String_fromChars("LDS"); //-----uncomment this one

	// See http://www.opcfoundation.org/UA/schemas/1.03/ServerCapabilities.csv
	// For a LDS server, you should only indicate the LDS capability.
	// If this instance is an LDS and at the same time a normal OPC UA server, you also have to indicate
	// the additional capabilities.
	// NOTE: UaExpert does not show LDS-only servers in the list.
	// See also: https://forum.unified-automation.com/topic1987.html

	// E.g. here we only set LDS, and you will not see it in UaExpert
	config->mdnsConfig.serverCapabilitiesSize = 2;
	caps = (UA_String *) UA_Array_new(2, &UA_TYPES[UA_TYPES_STRING]);
    config->mdnsConfig.mdnsServerName = UA_String_fromChars("LDSME");
    caps[0]=UA_String_fromChars("LDS");
    //caps[1]= UA_String_fromChars("Available");

    //printf("%s",x);
    //int getCount=getCountValue();
    printf("Count: %d\n",count);
    if(count==1){

    		caps[1]= UA_String_fromChars("Busy");
    		//printf("The server status is: %.*s\n", caps[1]);
    		}
    		else{
    		caps[1]= UA_String_fromChars("Available");
    			//printf("The server status is: %.*s\n", caps[0]);
    }


	config->mdnsConfig.serverCapabilities = caps;




	caps[0]= UA_String_fromChars("LDSME");

	caps[1]= UA_String_fromChars("Available");



	 timeout in seconds when to automatically remove a registered server from
	 * the list, if it doesn't re-register within the given time frame. A value
	 * of 0 disables automatic removal. Default is 60 Minutes (60*60). Must be
	 * bigger than 10 seconds, because cleanup is only triggered approximately
	 * every 10 seconds. The server will still be removed depending on the
	 * state of the semaphore file.
	// config->discoveryCleanupTimeout = 60*60;

	//UA_StatusCode retval = UA_Server_run(server, &running);
	UA_StatusCode retval = UA_Server_run_startup(server);

	if(retval!= UA_STATUSCODE_GOOD){
		UA_Server_delete(server);
		return retval;
	}
	int timestamp = time(0) + 1;

	while(running== true){

		UA_Server_run_iterate(server, true);
		 //caps[0]=UA_String_fromChars("Bangalore");
		 //caps[1]=UA_String_fromChars("Mangalore");
		if(time(0)>timestamp){


			timestamp= time(0) + 3;

		    //count= getCountValue();//check if the session has established
			//int Tcount=(int)serv.ss.sessionTimeoutCount;
			//double num;
			//num= static_cast<double>count;
			//printf("Num: %d\n",count);

			int count=getCountValue();

			//printf("Tcount:%d\n",Tcount);
			caps[1]= UA_String_fromChars("Available");
			if(count==1){
				caps[1]= UA_String_fromChars("Busy");
				printf("The server status is: %.*s\n", caps[1]);
			}
			else{
				printf("The server status is: %.*s\n", caps[1]);
			}

		   // config->mdnsConfig.serverCapabilities = caps;


	}
	}
	//config->mdnsConfig.serverCapabilities = caps;
	retval= UA_Server_run_shutdown(server);
	UA_Server_delete(server);
	return retval == UA_STATUSCODE_GOOD ? EXIT_SUCCESS : EXIT_FAILURE;

}*/
