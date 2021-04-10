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


char *A[] = {"opc.tcp://localhost:4840", "opc.tcp://helloworld:4840", "opc.tcp://helloworldagain:4844"};
char *B[] = {};
char *C[] ={};
int size = sizeof(A)/sizeof(A[0]);

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
   // for(int i =0; i<=5;i++){
        
        ret = connect(clientSocket,(struct sockaddr*)&serverAddr,sizeof(serverAddr));
       // printf("%d ", ret);
    
       // sleep(1);
    //}
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
    printf("%d\n",size);
    for(int i=0;i<size;i++){
            int length = strlen(A[i]);

            B[i] = A[i] + 10;
            printf("Hostname without AA binding : %s\n", B[i]);
            char *p = strrchr(B[i], ':');
            int port = strtoul(p+1, NULL, 10);
            printf("Extracted port : %d\n",port);
            //B[i][length-5]='\0';
            //Sprintf("Removed the port from AA : %s\n", B[i]);
         

         
           //removing the port
           printf("Original A[i] = %s\n",A[i]);
           C[i] = B[i][length-5];
           //B[i][length-5]='\0';
           printf("C[i] [length - 5] : next line =%s\n",C[i]);
           //without initial tcp binding
           C[i] = A[i] + 10;
          printf("Hostname after removing binding : %s\n", C[i]);


          // FIND IP OF THAT HOST
           if(i!=0){
                char ip_address[50];

                find_ip_address(C[i],ip_address);
       
                socketCommunication(ip_address,C[i],port);///,&B[i]);
        
          
            }
    }
}


