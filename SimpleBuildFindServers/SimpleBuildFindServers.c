/*
 ============================================================================
 Name        : SimpleBuildFindServers.c
 Author      : Rakshan
 Version     :
 Copyright   : Your copyright notice
 Description : Hello World in C, Ansi-style
 ============================================================================
 */

#include <stdlib.h>
#include "open625411.h"

#define DISCOVERY_SERVER_ENDPOINT "opc.tcp://192.168.0.207:4840"

int main(void) {


    /* Example for calling FindServers */
    UA_ApplicationDescription *applicationDescriptionArray = NULL;
    size_t applicationDescriptionArraySize = 0;

    UA_StatusCode retval;
    {
        UA_Client *client = UA_Client_new();
        UA_ClientConfig_setDefault(UA_Client_getConfig(client));
        retval = UA_Client_findServers(client, DISCOVERY_SERVER_ENDPOINT, 0, NULL, 0, NULL,
                                       &applicationDescriptionArraySize, &applicationDescriptionArray);
        UA_Client_delete(client);
    }
    if(retval != UA_STATUSCODE_GOOD) {
        UA_LOG_ERROR(UA_Log_Stdout, UA_LOGCATEGORY_SERVER, "Could not call FindServers service. "
                "Is the discovery server started? StatusCode %s", UA_StatusCode_name(retval));
        return EXIT_FAILURE;
    }

    // output all the returned/registered servers
    for(size_t i = 0; i < applicationDescriptionArraySize; i++) {
        UA_ApplicationDescription *description = &applicationDescriptionArray[i];
        printf("Server[%lu]: %.*s", (unsigned long) i, (int) description->applicationUri.length,
               description->applicationUri.data);
        printf("\n\tName: %.*s", (int) description->applicationName.text.length,
               description->applicationName.text.data);
        printf("\n\tApplication URI: %.*s", (int) description->applicationUri.length,
               description->applicationUri.data);
        printf("\n\tProduct URI: %.*s", (int) description->productUri.length,
               description->productUri.data);

        printf("\n\tStatus: %.*s", (int) description->gatewayServerUri.length,
                      description->gatewayServerUri.data);
        printf("\n\tType: ");
        switch(description->applicationType) {
            case UA_APPLICATIONTYPE_SERVER:
                printf("Server");
                break;
            case UA_APPLICATIONTYPE_CLIENT:
                printf("Client");
                break;
            case UA_APPLICATIONTYPE_CLIENTANDSERVER:
                printf("Client and Server");
                break;
            case UA_APPLICATIONTYPE_DISCOVERYSERVER:
                printf("Discovery Server");
                break;
            default:
                printf("Unknown");
        }
        printf("\n\tDiscovery URL:");
        for(size_t j = 0; j < description->discoveryUrlsSize; j++) {
        	if(j==1)
        		break;
            printf("%.*s",
                   (int) description->discoveryUrls[j].length,
                   description->discoveryUrls[j].data);
        }

     // printf("\n\tThe size: %ld",description->serverCapabilitiesSize);

//      printf("\n\tStatus:");
//      printf("\n\t\t %.*s",
//                           (int) description->serverCapabilities[1].length,
//                          description->serverCapabilities[1].data);
        printf("\n\n");
    }


    /*
     * Now that we have the list of available servers, call get endpoints on all of them
     */

//    printf("-------- Server Endpoints --------\n");
//
//    for(size_t i = 0; i < applicationDescriptionArraySize; i++) {
//        UA_ApplicationDescription *description = &applicationDescriptionArray[i];
//        if(description->discoveryUrlsSize == 0) {
//            UA_LOG_INFO(UA_Log_Stdout, UA_LOGCATEGORY_CLIENT,
//                        "[GetEndpoints] Server %.*s did not provide any discovery urls. Skipping.",
//                        (int)description->applicationUri.length, description->applicationUri.data);
//            continue;
//        }
//
//        printf("\nEndpoints for Server[%lu]: %.*s\n", (unsigned long) i,
//               (int) description->applicationUri.length, description->applicationUri.data);
//
//        UA_Client *client = UA_Client_new();
//        UA_ClientConfig_setDefault(UA_Client_getConfig(client));
//
//        char *discoveryUrl = (char *) UA_malloc(sizeof(char) * description->discoveryUrls[0].length + 1);
//        memcpy(discoveryUrl, description->discoveryUrls[0].data, description->discoveryUrls[0].length);
//        discoveryUrl[description->discoveryUrls[0].length] = '\0';
//
//        UA_EndpointDescription *endpointArray = NULL;
//        size_t endpointArraySize = 0;
//        //TODO: adapt to the new async getEndpoint
//        retval = UA_Client_getEndpoints(client, discoveryUrl, &endpointArraySize, &endpointArray);
//        UA_free(discoveryUrl);
//        if(retval != UA_STATUSCODE_GOOD) {
//            UA_Client_disconnect(client);
//            UA_Client_delete(client);
//            break;
//        }
//
//        for(size_t j = 0; j < endpointArraySize; j++) {
//            UA_EndpointDescription *endpoint = &endpointArray[j];
//            printf("\n\tEndpoint[%lu]:", (unsigned long) j);
//            printf("\n\t\tEndpoint URL: %.*s", (int) endpoint->endpointUrl.length, endpoint->endpointUrl.data);
//            printf("\n\t\tTransport profile URI: %.*s", (int) endpoint->transportProfileUri.length,
//                   endpoint->transportProfileUri.data);
//            printf("\n\t\tSecurity Mode: ");
//            switch(endpoint->securityMode) {
//            case UA_MESSAGESECURITYMODE_INVALID:
//                printf("Invalid");
//                break;
//            case UA_MESSAGESECURITYMODE_NONE:
//                printf("None");
//                break;
//            case UA_MESSAGESECURITYMODE_SIGN:
//                printf("Sign");
//                break;
//            case UA_MESSAGESECURITYMODE_SIGNANDENCRYPT:
//                printf("Sign and Encrypt");
//                break;
//            default:
//                printf("No valid security mode");
//                break;
//            }
//            printf("\n\t\tSecurity profile URI: %.*s", (int) endpoint->securityPolicyUri.length,
//                   endpoint->securityPolicyUri.data);
//            printf("\n\t\tSecurity Level: %d", endpoint->securityLevel);
//        }
//
//        UA_Array_delete(endpointArray, endpointArraySize, &UA_TYPES[UA_TYPES_ENDPOINTDESCRIPTION]);
//        UA_Client_delete(client);
//    }
//
//    printf("\n");
//
//    UA_Array_delete(applicationDescriptionArray, applicationDescriptionArraySize,
//                    &UA_TYPES[UA_TYPES_APPLICATIONDESCRIPTION]);

    return EXIT_SUCCESS;
}

