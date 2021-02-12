/*
 * ExecuteBusyInstance.c
 *
 *  Created on: Jan 19, 2021
 *      Author: rk469
 */
#include <signal.h>
#include <stdlib.h>
#include<time.h>
#include"open62541.h"
#include <signal.h>
#include <stdlib.h>

char *discovery_urls = NULL;
UA_Boolean running2 = true;
//
static void stopHandler(int sign) {
    UA_LOG_INFO(UA_Log_Stdout, UA_LOGCATEGORY_SERVER, "received ctrl-c");
    running2 = false;
}

static void Routine_run(UA_Server *server, UA_Client *clientRegister,UA_ServerConfig *config,
		char *discovery_urls,UA_String *caps,UA_UInt64 callbackId){
	UA_Server_run_shutdown(server);
	UA_StatusCode retval;
	retval = UA_Server_unregister_discovery(server, clientRegister);
	if(retval != UA_STATUSCODE_GOOD){
		UA_LOG_ERROR(UA_Log_Stdout, UA_LOGCATEGORY_SERVER,
					 "Could not unregister server from discovery server. "
					 "StatusCode %s", UA_StatusCode_name(retval));}
	else{
			UA_LOG_ERROR(UA_Log_Stdout, UA_LOGCATEGORY_SERVER,
			"Unregistration Successfull. "
			"StatusCode %s", UA_StatusCode_name(retval));
	}
	UA_Server_removeCallback(server, callbackId);
	caps[1]= UA_String_fromChars("Busy");
	sleep(1);
	main();
}

static void serverOnNetworkCallback(const UA_ServerOnNetwork *serverOnNetwork, UA_Boolean isServerAnnounce,
                        UA_Boolean isTxtReceived, void *data) {

    if(discovery_urls != NULL || !isServerAnnounce) {
        UA_LOG_DEBUG(UA_Log_Stdout, UA_LOGCATEGORY_SERVER,
                     "serverOnNetworkCallback called, but discovery URL "
                     "already initialized or is not announcing. Ignoring.");
       return; // we already have everything we need or we only want server announces
   }

    if(!isTxtReceived)
        return; // we wait until the corresponding TXT record is announced.
                // Problem: how to handle if a Server does not announce the
                // optional TXT?

    // here you can filter for a specific LDS server, e.g. call FindServers on
   // the serverOnNetwork to make sure you are registering with the correct
    // LDS. We will ignore this for now
   UA_LOG_INFO(UA_Log_Stdout, UA_LOGCATEGORY_SERVER, "Another server announced itself on %.*s",
               (int)serverOnNetwork->discoveryUrl.length, serverOnNetwork->discoveryUrl.data);
//
    if(discovery_urls != NULL)
       UA_free(discovery_urls);
    discovery_urls = (char*)UA_malloc(serverOnNetwork->discoveryUrl.length + 1);
    memcpy(discovery_urls, serverOnNetwork->discoveryUrl.data, serverOnNetwork->discoveryUrl.length);
    discovery_urls[serverOnNetwork->discoveryUrl.length] = 0;
}
void ExecuteBusyInstance(){
	UA_Server *server = UA_Server_new();
	    UA_ServerConfig *config = UA_Server_getConfig(server);
	    // use port 0 to dynamically assign port
	    UA_ServerConfig_setMinimal(config, 4841, NULL);



	    config->applicationDescription.applicationType = UA_APPLICATIONTYPE_SERVER;
	    UA_String_clear(&config->applicationDescription.applicationUri);
	    config->applicationDescription.applicationUri =
	        UA_String_fromChars("urn:open62541.example.server_multicast");

	    // Enable the mDNS announce and response functionality
	    config->mdnsEnabled = true;

	    config->mdnsConfig.mdnsServerName = UA_String_fromChars("Crane Multicast Server");

	    //setting custom outbound interface
	    config->mdnsInterfaceIP = UA_String_fromChars("0.0.0.0"); //this line will produce an error and set the interface to 0.0.0.0

	    config->mdnsConfig.serverCapabilitiesSize = 2;
	    UA_String *caps = (UA_String *) UA_Array_new(2, &UA_TYPES[UA_TYPES_STRING]);
	    int count= getCountValue();
	    //printf("%s",caps[1]);
	    caps[0] = UA_String_fromChars("LDS");
	    if(count!=0)
	    	caps[1] = UA_String_fromChars("Busy");
	    else
	    	caps[1]=UA_String_fromChars("Available");
	    config->mdnsConfig.serverCapabilities = caps;


	    // Start the server and call iterate to wait for the multicast discovery of the LDS
	    UA_StatusCode retval = UA_Server_run_startup(server);

	    // callback which is called when a new server is detected through mDNS
	    // needs to be set after UA_Server_run_startup or UA_Server_run
	    UA_Server_setServerOnNetworkCallback(server, serverOnNetworkCallback, NULL);

	    if(retval != UA_STATUSCODE_GOOD) {
	        UA_LOG_ERROR(UA_Log_Stdout, UA_LOGCATEGORY_SERVER,
	                     "Could not start the server. StatusCode %s",
	                     UA_StatusCode_name(retval));
	        UA_Server_delete(server);
	        UA_free(discovery_urls);

	    }
	    UA_LOG_INFO(UA_Log_Stdout, UA_LOGCATEGORY_SERVER,
	                "Server started. Waiting for announce of LDS Server.");
	    while (running2 && discovery_urls == NULL)
	        UA_Server_run_iterate(server, true);
	    if(!running2) {
	        UA_Server_delete(server);
	        UA_free(discovery_urls);

	    }

	    UA_Client *clientRegister = UA_Client_new();
	    UA_ClientConfig_setDefault(UA_Client_getConfig(clientRegister));

	    UA_UInt64 callbackId;
	    retval = UA_Server_addPeriodicServerRegisterCallback(server, clientRegister, discovery_urls,
	                                                         10 * 60 * 1000, 500,&callbackId);

	    if(retval != UA_STATUSCODE_GOOD) {
	        UA_LOG_ERROR(UA_Log_Stdout, UA_LOGCATEGORY_SERVER,
	                     "Could not create periodic job for server register. StatusCode %s",
	                     UA_StatusCode_name(retval));
	        UA_free(discovery_urls);
	        UA_Client_disconnect(clientRegister);
	        UA_Client_delete(clientRegister);
	        UA_Server_delete(server);

	    }
	    printf("Count = %d\n",count);
	    UA_Boolean isDone= false;
	    int timestamp = time(0) + 1;
	    while(running2== true){
				UA_Server_run_iterate(server, true);
				  if(count!=1){
						if(time(0)>timestamp){
						   timestamp= time(0) + 3;
						   int count= getCountValue();
						   printf("Num: %d\n",count);
						   if(count==1 && isDone == false){
							   Routine_run(server,clientRegister,config,discovery_urls,caps,callbackId);

							   isDone=true;
						    }
						   else{
							   continue;
						   }
						}
				   }
				 else{
					 continue;
				 }

	    }
		UA_Server_run_shutdown(server);

	    //UA_Server_run_shutdown(server);

	    // UNregister the server from the discovery server.
	    retval = UA_Server_unregister_discovery(server, clientRegister);
	    if(retval != UA_STATUSCODE_GOOD)
	        UA_LOG_ERROR(UA_Log_Stdout, UA_LOGCATEGORY_SERVER,
	                     "Could not unregister server from discovery server. "
	                     "StatusCode %s", UA_StatusCode_name(retval));








	    UA_Server_removeCallback(server, callbackId);
	    UA_free(discovery_urls);

	    UA_Client_disconnect(clientRegister);
	    UA_Client_delete(clientRegister);
	    UA_Server_delete(server);

	}


