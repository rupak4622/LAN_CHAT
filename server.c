//  gcc server.c -o server.exe -lws2_32

#include <stdio.h>  
#include <winsock2.h>  // its is used for socket programming in windows 
#include <windows.h>  // it is used for creating threads in windows
#include <stdlib.h>   // it is used for exit function
#include <string.h>   // it is used for string manipulation functions like strcpy and sprintf

#pragma comment(lib,"ws2_32.lib")  // it is used to tell the compiler to link the ws2_32.lib library which contains the implementation of the Winsock functions used in this program

SOCKET clients[10];               // it is an array to store the client sockets upto 10 clients can connect to the server at a time
char clientNames[10][50];           // it is a 2d array to to store the names of clients in which max 10 number of clients and max 50 charcters for each name can be stored
int clientCount = 0;              // Keeps track of how many clients are connected


DWORD WINAPI receiveClient(LPVOID clientSocket){    
    //  - LPVOID arg means the thread receives one argument, and it’s a generic pointer (void*).
    //  - The fourth parameter (LPVOID)clientSock is the argument you pass to the thread.
    

    SOCKET sock = (SOCKET)clientSocket;      //  - means inside receiveClient, the parameter clientSocket will hold this value.
    char buffer[1024];    // Buffer to store messages from clients
    char name[50];        // Buffer to store the name of the client

    int nameLen = recv(sock,name,sizeof(name),0);  // Receive client’s name
    name[nameLen] = '\0';                         // End the string properly

    strcpy(clientNames[clientCount-1], name);    // Save client’s name  

    char joinmsg[100];                            // create a message to notify other clients that a new client has joined the chat
    sprintf(joinmsg,">> %s joined the chat",name);

    for(int i=0;i<clientCount;i++)
        send(clients[i],joinmsg,strlen(joinmsg),0);      // Broadcast the join message to all clients   

    while(1){
        int bytes = recv(sock, buffer, sizeof(buffer), 0);

        if(bytes<=0){
            printf("\n%s disconnected\n> ",name);

            char leavemsg[100];
            sprintf(leavemsg,">> %s left the chat",name);

            for(int i=0;i<clientCount;i++)
                send(clients[i],leavemsg,strlen(leavemsg),0);

            closesocket(sock);
            return 0;
        }

        buffer[bytes]='\0';

        printf("\n%s\n> ",buffer);
        fflush(stdout);

        for(int i=0;i<clientCount;i++)
            send(clients[i],buffer,strlen(buffer),0);
    }
}

DWORD WINAPI serverSend(LPVOID arg){
    char msg[1024];       // 

    while(1){
        printf("> ");
        scanf(" %[^\n]", msg);     

        char final[1100];            //             
        sprintf(final,"SERVER: %s",msg);

        for(int i=0;i<clientCount;i++)             // brodcast the message to all clients
            send(clients[i],final,strlen(final),0);
    }
}

int main(){
    system("color 0A");

    WSADATA wsa;
    WSAStartup(MAKEWORD(2,2), &wsa);

    SOCKET serverSock, clientSock;
    struct sockaddr_in server, client;
    int c = sizeof(struct sockaddr_in);

    serverSock = socket(AF_INET, SOCK_STREAM, 0);

    server.sin_family = AF_INET;
    server.sin_addr.s_addr = INADDR_ANY;
    server.sin_port = htons(5000);

    bind(serverSock,(struct sockaddr*)&server,sizeof(server));
    listen(serverSock,5);

    printf("=================================\n");
    printf("       CHAT IS LIVE ...\n");
    printf("=================================\n");

    CreateThread(NULL,0,serverSend,NULL,0,NULL);

    while(1){
        clientSock = accept(serverSock,(struct sockaddr*)&client,&c);

        clients[clientCount++] = clientSock;

        printf("Client connected!\n");

        CreateThread(NULL,0,receiveClient,(LPVOID)clientSock,0,NULL);
    }
}
