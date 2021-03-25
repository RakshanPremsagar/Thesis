/*
 ============================================================================
 Name        : modification_regServer.c
 Author      : Rakshan
 Version     :
 Copyright   : Your copyright notice
 Description : Hello World in C, Ansi-style
 ============================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include"open625411.h"

/* This work is licensed under a Creative Commons CCZero 1.0 Universal License.
 * See http://creativecommons.org/publicdomain/zero/1.0/ for more information. */
/*
 * A simple server instance which registers with the discovery server (see server_lds.c).
 * Before shutdown it has to unregister itself.
 */


#define DISCOVERY_SERVER_ENDPOINT "opc.tcp://192.168.0.207:4840"

UA_Boolean running = true;

static void stopHandler(int sign) {
    UA_LOG_INFO(UA_Log_Stdout, UA_LOGCATEGORY_SERVER, "received ctrl-c");
    running = false;
}


static void Routine_run(UA_Server *server, UA_Client *clientRegister,UA_ServerConfig *config,
		char *discovery_url,UA_UInt64 callbackId,char *status){
	//UA_Server_run_shutdown(server);
	UA_StatusCode retval;
	retval = UA_Server_unregister_discovery(server, clientRegister);
	if(retval != UA_STATUSCODE_GOOD){
		UA_LOG_INFO(UA_Log_Stdout, UA_LOGCATEGORY_SERVER,
					 "Could not unregister server from discovery server. "
					 "StatusCode %s", UA_StatusCode_name(retval));}
	else{
		UA_LOG_INFO(UA_Log_Stdout, UA_LOGCATEGORY_SERVER,
			"Unregistration Successfull. "
			"StatusCode %s", UA_StatusCode_name(retval));
	}
	UA_Server_removeCallback(server, callbackId);
    sleep(1);



	config->applicationDescription.status= UA_String_fromChars(status);


	retval = UA_Server_register_discovery(server, clientRegister,NULL);
	    if(retval != UA_STATUSCODE_GOOD)
	        UA_LOG_INFO(UA_Log_Stdout, UA_LOGCATEGORY_SERVER,
	                     "Could not register server to discovery server. "
	                     "StatusCode %s", UA_StatusCode_name(retval));
	    else{
	    	UA_LOG_INFO(UA_Log_Stdout, UA_LOGCATEGORY_SERVER,
	    			"registration Successfull. "
	    			"StatusCode %s", UA_StatusCode_name(retval));
	    	}
}

int main(int argc, char **argv) {
    signal(SIGINT, stopHandler); /* catches ctrl-c */
    signal(SIGTERM, stopHandler);
    signal(SIGHUP,stopHandler);
    //signal(SIGKILL,stopHandler);
    char *status=NULL;

    UA_Server *server = UA_Server_new();
    UA_ServerConfig *config = UA_Server_getConfig(server);
    // use port 0 to dynamically assign port
    UA_ServerConfig_setMinimal(config, 4841, NULL);

    UA_String_clear(&config->applicationDescription.applicationUri);


    config->applicationDescription.applicationUri =
       UA_String_fromChars("Sample_Crane_Server");

    //config->applicationDescription.serverCapabilitiesSize = 2;
    //config->mdnsConfig.mdnsServerName = UA_String_fromChars("CraneServer");
    // See http://www.opcfoundation.org/UA/schemas/1.04/ServerCapabilities.csv
    //config.serverCapabilitiesSize = 1;
    //UA_String caps = UA_String_fromChars("LDS");
    //config.serverCapabilities = &caps;
    config->applicationDescription.status=UA_String_fromChars("Starting...");

    UA_StatusCode retval = UA_Server_run_startup(server);

    while (running && DISCOVERY_SERVER_ENDPOINT == NULL)
            UA_Server_run_iterate(server, true);
        if(!running) {
            UA_Server_delete(server);
            UA_free(DISCOVERY_SERVER_ENDPOINT);
            return EXIT_FAILURE;
        }

    UA_Client *clientRegister = UA_Client_new();
    UA_ClientConfig_setDefault(UA_Client_getConfig(clientRegister));

    // periodic server register after 10 Minutes, delay first register for 500ms
    UA_UInt64 callbackId;
    UA_StatusCode retval2 =
        UA_Server_addPeriodicServerRegisterCallback(server, clientRegister, DISCOVERY_SERVER_ENDPOINT,
                                                    10 * 60 * 1000, 500, &callbackId);
    // UA_StatusCode retval = UA_Server_addPeriodicServerRegisterJob(server,
    // "opc.tcp://localhost:4840", 10*60*1000, 500, NULL);
    if(retval2 != UA_STATUSCODE_GOOD) {
        UA_LOG_ERROR(UA_Log_Stdout, UA_LOGCATEGORY_SERVER,
                     "Could not create periodic job for server register. StatusCode %s",
                     UA_StatusCode_name(retval));
        UA_Client_disconnect(clientRegister);
        UA_Client_delete(clientRegister);
        UA_Server_delete(server);
        return EXIT_FAILURE;
    }
    UA_Boolean isDone= false;
    UA_Boolean isChange=false;
    int timestamp = time(0) + 1;
    while (running){
    	UA_Server_run_iterate(server, true);
    	    				if(time(0)>timestamp){
    	    					timestamp= time(0) +1;
    	    					int count= getCountValue();
    	    					printf("Num: %d\n",count);
    	    					if(count==1 && isDone == false){
    	    							   status="Busy";
    	    							   Routine_run(server,clientRegister,config,DISCOVERY_SERVER_ENDPOINT,callbackId,status);

    	    							   isDone=true;
    	    							   isChange=true;
    	    					}
    	    					else if (count==0 && isChange == true){
    	    							   status="Available";
    	    							   Routine_run(server,clientRegister,config,DISCOVERY_SERVER_ENDPOINT,callbackId,status);
    	    							   isDone = false;
    	    							   isChange=false;
    	    					}
    	    					else {
    	    							continue;  //here it is wrong to use as it would unnecessarily deregister
    	    					}



    	    				}
    	    				 else{
    	    					 continue;
    	    				 }
    	    		}


    	    UA_Server_run_shutdown(server);

    	    // UNregister the server from the discovery server.
    	    retval = UA_Server_unregister_discovery(server, clientRegister);
    	    if(retval != UA_STATUSCODE_GOOD)
    	        UA_LOG_ERROR(UA_Log_Stdout, UA_LOGCATEGORY_SERVER,
    	                     "Could not unregister server from discovery server. "
    	                    "StatusCode %s", UA_StatusCode_name(retval));


    	    UA_Server_removeCallback(server, callbackId);


    	    UA_Client_disconnect(clientRegister);
    	    UA_Client_delete(clientRegister);
    	    UA_Server_delete(server);
    	    return retval == UA_STATUSCODE_GOOD ? EXIT_SUCCESS : EXIT_FAILURE;
    	}




//    if(retval != UA_STATUSCODE_GOOD) {
//        UA_LOG_ERROR(UA_Log_Stdout, UA_LOGCATEGORY_SERVER,
//                     "Could not start the server. StatusCode %s",
//                     UA_StatusCode_name(retval));
//        UA_Client_disconnect(clientRegister);
//        UA_Client_delete(clientRegister);
//        UA_Server_delete(server);
//        return EXIT_FAILURE;
//    }
//
//    // Unregister the server from the discovery server.
//    retval = UA_Server_unregister_discovery(server, clientRegister);
//    //retval = UA_Server_unregister_discovery(server, "opc.tcp://localhost:4840" );
//    if(retval != UA_STATUSCODE_GOOD)
//        UA_LOG_ERROR(UA_Log_Stdout, UA_LOGCATEGORY_SERVER,
//                     "Could not unregister server from discovery server. StatusCode %s",
//                     UA_StatusCode_name(retval));
//
//    UA_Server_removeCallback(server, callbackId);
//
//    UA_Client_disconnect(clientRegister);
//    UA_Client_delete(clientRegister);
//    UA_Server_delete(server);
//    return retval == UA_STATUSCODE_GOOD ? EXIT_SUCCESS : EXIT_FAILURE;
//}
