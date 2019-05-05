#include <stdio.h> /* for printf() and fprintf() */
#include <sys/socket.h> /* for socket(), connect(), send(), and recv() */
#include <arpa/inet.h> /* for sockaddr_in and inet_addr() */
#include <stdlib.h> /* for atoi() */
#include <string.h> /* for memset() */
#include <unistd.h> /* for close() */

#define BufferSize 128 /* Size of receive buffer */
#define hangingLimit 5

void ClientHandle(int sockForClientet);
void Menu();
int mesgLog;

int main(int argc, char *argv[])
{
    int sock, sockForServer, sockForClient;
    struct sockaddr_in echoServAddr; /* Echo server address */
    unsigned short echoServPort; /* Echo server port */
    char *servlP; /* Server IP address (dotted quad) */
    char *echoString; /* String to send to echo server */
    char echoBuffer[BufferSize]; /* Buffer for echo string */
    unsigned int echoStringLen, lengthForClient;
    int bytesRcvd, totalBytesRcvd; /* Bytes read in single recv() and total bytes read */
    
    char celector[2], IP[128], rcvr[128], mesg[128], user_name_login[128], user_password_login[128];
    int port, tempPort;
    bool linked = false;

    printf("~~~WELCOME TO ALBERTS CLIENT/SERVER PROJECT~~~\n");   
    
    while(1)
    {
        Menu(); 
        scanf("%s", celector); 

        if((strcmp(celector, "0"))==0) 
        { 
            if(linked == false)
            {
                printf("Input IP address: ");
                scanf("%s", IP);
                printf("Input Port number: ");
                scanf("%d", &port);
                printf("\n~~~Establishing a connection~~~\n");
				sock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
                memset(&echoServAddr, 0, sizeof(echoServAddr)); 
                echoServAddr.sin_family = AF_INET; 
                echoServAddr.sin_addr.s_addr = inet_addr(IP);  
                echoServAddr.sin_port = htons(port);
                connect(sock, (struct sockaddr *) &echoServAddr, sizeof(echoServAddr));
                printf("---The connection has been established---\n");
                linked = true;
                send(sock, celector, strlen(celector) , 0);
                memset(echoBuffer, 0, sizeof(echoBuffer));
                bytesRcvd=recv(sock, echoBuffer, sizeof(echoBuffer), 0); 
                printf("%s",echoBuffer);  
                printf("Login Name: ");
                scanf("%s", user_name_login);
                send(sock, user_name_login, strlen(user_name_login) , 0); 
                memset(user_name_login,0,sizeof(user_name_login));
                printf("Password: ");
                scanf("%s", user_password_login);
                send(sock, user_password_login, strlen(user_password_login) , 0); 
                memset(user_password_login,0,sizeof(user_password_login));
                memset(echoBuffer, 0, sizeof(echoBuffer));
                bytesRcvd=recv(sock, echoBuffer, sizeof(echoBuffer), 0); 
                printf("%s",echoBuffer); 
            }
			else
            {printf("\nYou are already loged in\n");}
        }
        else if(strcmp(celector, "1")==0)
        { 
            if(linked == true)
            {
                send(sock, celector, strlen(celector) , 0);
                printf("\nUser List: ");
                memset(echoBuffer, 0, sizeof(echoBuffer));
                bytesRcvd=recv(sock, echoBuffer, sizeof(echoBuffer), 0);              
                printf("%s",echoBuffer);
            }
            else
            {printf("\nConnect to the server first!\n");}
        }
        else if(strcmp(celector, "2")==0)
        {
        	if(linked == true) 
        	{
                send(sock, celector, strlen(celector) , 0) ;
        		memset(echoBuffer, 0, sizeof(echoBuffer));
        		bytesRcvd=recv(sock, echoBuffer, sizeof(echoBuffer), 0);
        		printf("%s\n",echoBuffer );
        		printf("\nInput Recever: ");
        		scanf("%s", rcvr);
        		rcvr[strlen(rcvr)]='\0';
				send(sock, rcvr, strlen(rcvr) , 0);
                memset(rcvr,0,sizeof(rcvr));
				memset(echoBuffer, 0, sizeof(echoBuffer));
        		bytesRcvd=recv(sock, echoBuffer, sizeof(echoBuffer), 0);
        		printf("%s", echoBuffer); 
      		
      			for(int i;((i = getchar()) != '\n' && i != EOF);)
      			{ }

        		fgets(mesg,128,stdin);
        		mesg[strlen(mesg)-1]='\0';
        		send(sock, mesg, strlen(mesg) , 0) ;
                memset(mesg, 0, sizeof(mesg));
                printf("\nMessage Sent!");
        	}
			else
        	{printf("\nLink to server to send message!\n");}
        }
        else if(strcmp(celector, "3")==0)
        {
        	send(sock, celector, strlen(celector) , 0);
        	memset(echoBuffer, 0, sizeof(echoBuffer));
        	bytesRcvd=recv(sock, echoBuffer, sizeof(echoBuffer), 0);
        	printf("%s", "New Message -> ");
        	printf("%s", echoBuffer); 
            printf("\n");   
        }
        else if(strcmp(celector, "4")==0)
        { 
        	if(linked == true)
			{
				send(sock, celector, strlen(celector) , 0);
				linked = false;
				close(sock);
	        }
			
        	sockForServer = 0;
			sockForClient = 0;
			lengthForClient = 0; 
    		struct sockaddr_in echoServAddr; 
    		struct sockaddr_in echoClntAddr; 
            printf("Imput a Port Number: ");
            scanf("%d", &echoServPort);
            printf("Awating Connection....");
            memset(echoBuffer, 0, sizeof(echoBuffer));
            sockForServer = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);    
            memset(&echoServAddr, 0, sizeof(echoServAddr)); 
            echoServAddr.sin_family = AF_INET; 
            echoServAddr.sin_addr.s_addr = htonl(INADDR_ANY); 
            echoServAddr.sin_port = htons(echoServPort); 
            bind(sockForServer, (struct sockaddr *)&echoServAddr, sizeof(echoServAddr));    
            listen(sockForServer, hangingLimit);
            
			while(1)
			{  
                lengthForClient = sizeof(echoClntAddr);
                sockForClient = accept(sockForServer, (struct sockaddr *) &echoClntAddr, &lengthForClient); 
                printf("Handling client %s\n", inet_ntoa(echoClntAddr.sin_addr));
                ClientHandle (sockForClient); 
            }
        }
        else if(strcmp(celector, "5")==0)
        { 
            printf("Input IP address: ");
            scanf("%s", IP);
            printf("Input Port number: ");
            scanf("%d", &tempPort);
            printf("Establishing a new connection. \n");
            sock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
            memset(&echoServAddr, 0, sizeof(echoServAddr));
            echoServAddr.sin_family = AF_INET; 
            echoServAddr.sin_addr.s_addr = inet_addr(IP); 
            echoServAddr.sin_port = htons(tempPort); 
            connect(sock, (struct sockaddr *) &echoServAddr, sizeof(echoServAddr));
            printf("New connection has been established!\n");       
			printf("\n\nInsert <bye> to exit\n\n");

			while(1)
			{    	
		        memset(echoBuffer, 0, sizeof(echoBuffer));
				mesgLog=recv(sock, echoBuffer, sizeof(echoBuffer), 0);       	
				printf("%s",echoBuffer);
				memset(echoBuffer, 0, sizeof(echoBuffer));
				fgets(echoBuffer,128,stdin);
				echoBuffer[strlen(echoBuffer)] = '\0';
				send(sock, echoBuffer, strlen(echoBuffer) , 0);
				
				if(strcmp(echoBuffer,"bye\n")==0)
				{	
					memset(echoBuffer, 0, sizeof(echoBuffer));
					close(sock);
					printf("\n\nConnection Ended!\n\n");
					break;
				}
				memset(echoBuffer, 0, sizeof(echoBuffer));
			}
        }
    }
}

void Menu()
{  
    printf("\n0 - Connect to Server\n");
    printf("1 - Get the user list\n");
    printf("2 - Send a mesg\n");
    printf("3 - Get my mesgs\n");
    printf("4 - Initiate a chat with my friend\n");
    printf("5 - Chat with my friend\n");
    printf("\nInput the number from the options listed above: ");
}

void ClientHandle(int sockForClientet)
{
	printf("\nHandaling Client\n");
    char echoBuffer[BufferSize]; 
	printf("\nInput 'bye' to quit\n");    

	while(1)
	{
        printf("\n");
		memset(echoBuffer, 0, sizeof(echoBuffer));
		fgets(echoBuffer,128,stdin);
        echoBuffer[strlen(echoBuffer)]='\0';
        send(sockForClientet, echoBuffer, strlen(echoBuffer) , 0);
        
        if(strcmp(echoBuffer,"bye\n")==0)
		{	
			memset(echoBuffer, 0, sizeof(echoBuffer));
			close(sockForClientet);
			printf("\nYou have been disconected\n");
			exit(1);
		}   
        memset(echoBuffer, 0, sizeof(echoBuffer)); 
        mesgLog=recv(sockForClientet, echoBuffer, sizeof(echoBuffer), 0);	
       	printf("%s",echoBuffer);
	}
}
