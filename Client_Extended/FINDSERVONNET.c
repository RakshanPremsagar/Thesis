
#include"open62541.h"
#include <stdlib.h>
#include<stdio.h>
#include<string.h>
#include<unistd.h>
#include<sys/socket.h>
#include<sys/types.h>
#include<netinet/in.h>
#include<arpa/inet.h>
#include <errno.h>
#include <netdb.h>
#include<time.h>



#define DISCOVERY_SERVER_ENDPOINT "opc.tcp://localhost:4840"
 char *A[] ={};
 char *B[] = {};
 char *C[] ={};
 char *ip_copy = NULL;


int find_ip_address(char *hostname, char *ip_address)
{
      struct hostent *host_name;
      struct in_addr **ipaddress;
      int count;
      if((host_name = gethostbyname(hostname)) == NULL)
      {
            herror("\nIP Address Not Found\n");
            return 1;
      }
      else
      {
            ipaddress = (struct in_addr **) host_name->h_addr_list;
            for(count = 0; ipaddress[count] != NULL; count++)
            {
                  strcpy(ip_address, inet_ntoa(*ipaddress[count]));
                  return 0;
            }
      }
      return 1;
}

int ret;

void socketCommunication(char *ip_address,char *hostname, int port){
     int clientSocket,ret;
    struct  sockaddr_in serverAddr;
    char buffer[1024];
 
    
    clientSocket = socket(AF_INET,SOCK_STREAM ,0);
    if(clientSocket<0){
        printf("Error in connection \n");
        exit(1);
    }

    //printf("Client socket is created\n");

    memset(&serverAddr,'\0',sizeof(serverAddr));
    // printf("MEMSET DONE\n");

    serverAddr.sin_port = htons(port);
    //printf("PORT DONE\n");
    serverAddr.sin_family=AF_INET;
    //printf("afnet DONE\n");
    serverAddr.sin_addr.s_addr=inet_addr(ip_address);

    //printf("%s\n", *ip_address);

    //printf("Config done: \n");

    /******TRYING TO MAKE NONBLOCKING SOCKET*************/
   // u_long optval = 0;
   // int nret = ioctlsocket(clientSocket,FIONBIO,&optval);
    /******TRYING TO MAKE NONBLOCKING SOCKET*************/ 
   
        
    ret = connect(clientSocket,(struct sockaddr*)&serverAddr,sizeof(serverAddr));
    fprintf(stdout, "Error: %s\n", strerror(errno));	
       // printf("%d ", ret);
    if(ret<0){
        printf("\nLOOKS LIKE NETWORK CONNECTION HAS FAILED. HAVE A LOOK AT THE NETWORK CONNECTIVITY at host : %s\n",hostname);
        printf("\n----Updated Status Information----:\n");
        printf("Discovery URL : opc.tcp://%s:%d\n",hostname,port);
        printf("Status:CONNECTON TIMED OUT\n");
        printf("\n");
    //     exit(1);
    }
   // printf("Connected to Server\n");  
}




int main(void) {

    /*
     * Example for calling FindServersOnNetwork
     */

    	time_t begin, end;
        UA_ServerOnNetwork *serverOnNetwork = NULL;
        size_t serverOnNetworkSize = 0;
        UA_Client *client = UA_Client_new();
        UA_ClientConfig_setDefault(UA_Client_getConfig(client));
        UA_StatusCode retval = UA_Client_findServersOnNetwork(client, DISCOVERY_SERVER_ENDPOINT, 0, 0,
                                                              0, NULL, &serverOnNetworkSize, &serverOnNetwork);
        if(retval != UA_STATUSCODE_GOOD) {
            UA_LOG_ERROR(UA_Log_Stdout, UA_LOGCATEGORY_SERVER,
                         "Could not call FindServersOnNetwork service. "
                         "Is the discovery server started? StatusCode %s",
                         UA_StatusCode_name(retval));
            UA_Client_delete(client);
            return EXIT_FAILURE;
        }

        //*******
        // output all the returned/registered servers
        for(size_t i = 0; i < serverOnNetworkSize; i++) {
            UA_ServerOnNetwork *server = &serverOnNetwork[i];
            printf("Server[%lu]: %.*s", (unsigned long) i,
                   (int) server->serverName.length, server->serverName.data);
            printf("\n\tRecordID: %d", server->recordId);
            printf("\n\tDiscovery URL: %.*s", (int) server->discoveryUrl.length,
                   server->discoveryUrl.data);



            printf("\n\tCapabilities: ");
            /*for(size_t j = 0; j < server->serverCapabilitiesSize; j++) {
                printf("%.*s,", (int) server->serverCapabilities[j].length,
                       server->serverCapabilities[j].data);
            }*/

            //added below
            printf("%.*s", (int) server->serverCapabilities[0].length,
                                   server->serverCapabilities[0].data);
            printf("\n\tStatus: ");
            printf("%.*s", (int) server->serverCapabilities[1].length,
                                               server->serverCapabilities[1].data);

        
            printf("\n\n");
        }
        //*******************************
          
        printf("--Checking for network connectivity--\n");
          time(&begin);
        for(size_t i = 0; i < serverOnNetworkSize; i++) {
           UA_ServerOnNetwork *server = &serverOnNetwork[i];
           A[i] = (char *)UA_malloc(server->discoveryUrl.length+1);
           memcpy(A[i],server->discoveryUrl.data,server->discoveryUrl.length);
           A[i][server->discoveryUrl.length] = 0;
           int length = strlen(A[i]);

          //new addition to extract port
            B[i] = A[i] + 10;
            //printf("Hostname without AA binding : %s\n", B[i]);
            char *p = strrchr(B[i], ':');
            int port = strtoul(p+1, NULL, 10);
            //printf("Extracted port : %d\n",port);
            //B[i][length-5]='\0';
            //Sprintf("Removed the port from AA : %s\n", B[i]);
         

         
           //removing the port
           //printf("Original A[i] = %s\n",A[i]);
           A[i][length-5]='\0';
           printf("A[i] [length - 5] : next line =%s\n",A[i]);
           //without initial tcp binding
           C[i] = A[i] + 10;
          printf("Hostname after removing binding : %s\n", C[i]);
          printf("--------------------------------------\n");
          // FIND IP OF THAT HOST
           if(i!=0){
            char ip_address[50];

            find_ip_address(C[i],ip_address);
            //printf("\nIP Address of Hostname %s:\t%s\n", B[i], ip_address);
            //socketCommunication(&ip_address,&B[i]);
            
            socketCommunication(ip_address,C[i],port);///,&B[i]);
           //memcpy(hostname,A[i],strlen(A[i])-5);
           //printf("%s\n", hostname);
          

           }


        }
          printf("--Checks done!--\n");
             time(&end);
          time_t elapsed = end - begin;
   	
    	  printf("Time measured: %ld seconds.\n", elapsed);
    




         printf("\n\n");
        


        UA_Array_delete(serverOnNetwork, serverOnNetworkSize,
                        &UA_TYPES[UA_TYPES_SERVERONNETWORK]);

        }

