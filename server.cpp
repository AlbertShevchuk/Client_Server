#include <stdio.h> /* for printf() and fprintf() */
#include <sys/socket.h> /* for socket(), bind(), and connect() */
#include <arpa/inet.h> /* for sockaddr_in and inet_ntoa() */
#include <stdlib.h> /* for atoi() */
#include <string.h> /* for memset() */
#include <unistd.h> /* for close() */

#define BufferSize 128
#define MAXPENDING 5 /* Maximum outstanding connection requests */

void printcurrentIndex();
void HandleTCPClient(int clntSocket); /* TCP client handling function */

char ongoing[128], collector[128], holdMesg[128], *currentIndex="bob and alice\n";
int indicator = 0, mesgLimit=128, userTotal=2, counter = 0;
bool linked = false;

struct userIndex
{
    char *user_name_login, *user_name_password, messages[128];
    int messageIndex;
};

struct userIndex userIndexArray[2];

int main(int argc, char *argv[])
{
    int servSock; /* Socket descriptor for server */
    int clntSock; /* Socket descriptor for client */
    struct sockaddr_in echoServAddr; /* Local address */
    struct sockaddr_in echoClntAddr; /* Client address */
    unsigned short echoServPort; /* Server port */
    unsigned int clntLen; /* Length of client address data structure */

    userIndexArray[0].user_name_login = "bob";
    userIndexArray[1].user_name_login="alice";

    userIndexArray[0].user_name_password = "000";
    userIndexArray[1].user_name_password="000";

    userIndexArray[0].messageIndex=0;
    userIndexArray[1].messageIndex=0;
    
    if (argc != 2) /* Test for correct number of arguments */
    {
        fprintf(stderr, "Usage: %s <Server Port>\n", argv[0]) ;
        exit(1);
    }

    echoServPort = atoi(argv[1]); /* First arg: local port */
    (servSock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP));

    /* Construct local address structure */
    memset(&echoServAddr, 0, sizeof(echoServAddr)); /* Zero out structure */
    echoServAddr.sin_family = AF_INET; /* Internet address family */
    echoServAddr.sin_addr.s_addr = htonl(INADDR_ANY); /* Any incoming interface */
    echoServAddr.sin_port = htons(echoServPort); /* Local port */

    /* Bind to the local address */
    bind(servSock, (struct sockaddr *)&echoServAddr, sizeof(echoServAddr));
    /* Mark the socket so it will listen for incoming connections */
	
    listen(servSock, MAXPENDING);

    while(1)
    {
        /* Set the size of the in-out parameter */
        clntLen = sizeof(echoClntAddr);
        /* Wait for a client to connect */
        (clntSock = accept(servSock, (struct sockaddr *) &echoClntAddr, &clntLen));
        printf("Incoming New Client: %s\n", inet_ntoa(echoClntAddr.sin_addr));
        HandleTCPClient (clntSock); 
    }
}

void HandleTCPClient(int clntSocket)
{
    char feedbackBuff[BufferSize], clientName[128], clientPassword[128], messageLog[128];
    int incomingCapacity;

    while(1)
    {
        memset(feedbackBuff, 0, sizeof(feedbackBuff));
        (incomingCapacity = recv(clntSocket, feedbackBuff, sizeof(feedbackBuff), 0));

        if((strcmp(feedbackBuff,"0"))==0)
        {
            memset(feedbackBuff, 0, sizeof(feedbackBuff));
            strcpy(feedbackBuff, "\nPlese enter Login information\n");
            send(clntSocket, feedbackBuff, strlen(feedbackBuff),0);
            memset(clientName, 0, sizeof(clientName));
            incomingCapacity=recv(clntSocket, clientName, sizeof(clientName),0);
            memset(clientPassword, 0, sizeof(clientPassword));
            incomingCapacity=recv(clntSocket, clientPassword, sizeof(clientPassword),0);
			counter = 0;
			
            while(counter<userTotal)
            {
                if(strcmp(userIndexArray[counter].user_name_login, clientName)==0 && strcmp(userIndexArray[counter].user_name_password, clientPassword)==0)
                {
					linked = true;
                    indicator = counter;
                    counter=userTotal;
                    strcpy(ongoing, clientName);
                    memset(feedbackBuff, 0, sizeof(feedbackBuff));
                    strcpy(feedbackBuff, "\nYou are logged in!\n");
					printf("Client is Loged In\n");
                    send(clntSocket, feedbackBuff, strlen(feedbackBuff),0);
                }
			counter++;
            }
        }
		else if((strcmp(feedbackBuff,"1"))==0)
        {
            send(clntSocket, currentIndex, strlen(currentIndex),0);
			printf("User List Requested\n");     
        }
        else if ((strcmp(feedbackBuff,"2"))==0)
        {
            memset(feedbackBuff, 0, sizeof(feedbackBuff));
            strcpy(feedbackBuff, "\n~~~Enter user to send message to~~~");        
            send(clntSocket, feedbackBuff, strlen(feedbackBuff),0);
            memset(feedbackBuff, 0, sizeof(feedbackBuff));
            incomingCapacity=recv(clntSocket, collector, sizeof(collector),0);
            printf("%s\n",collector );
            memset(feedbackBuff, 0, sizeof(feedbackBuff));
            strcpy(feedbackBuff, "\nType in your message: ");     
            send(clntSocket, feedbackBuff, strlen(feedbackBuff),0);   
            incomingCapacity=recv(clntSocket, holdMesg, sizeof(holdMesg),0);
            counter = 0;
			
                while(counter<userTotal)
                {
                    if(strcmp(userIndexArray[counter].user_name_login, collector)==0)
                    {
                        printf("New Message Recived!\n");
                        strcpy(userIndexArray[counter].messages, holdMesg);
                        userIndexArray[counter].messageIndex++;
                    }
				counter++;
                }
                memset(collector,0,sizeof(collector));
                memset(holdMesg, 0, sizeof(holdMesg)); 
        }
        else if (strcmp(feedbackBuff, "3")==0)
        {  
            memset(feedbackBuff, 0, sizeof(feedbackBuff));
            strcpy(feedbackBuff, userIndexArray[indicator].messages);
            send(clntSocket, feedbackBuff, strlen(feedbackBuff),0);
            printf("Client sent a message\n");
        }
        else if(strcmp(feedbackBuff, "4")==0)
        {
			linked = false;
			indicator=0;
            memset(ongoing,0, sizeof(ongoing));
            close(clntSocket);
            printf("Client initilized a new server\n");
            break;
        }
    }
}



