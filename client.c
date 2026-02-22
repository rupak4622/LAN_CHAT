#include <stdio.h>
#include <string.h>
#include <winsock2.h>
#include <windows.h>
#include <stdlib.h>

#pragma comment(lib,"ws2_32.lib")

SOCKET sock;
char name[50];

DWORD WINAPI receiveMessages(LPVOID arg){
    char buffer[1024];

    while(1){
        int recv_size = recv(sock, buffer, sizeof(buffer), 0);

        if(recv_size<=0){
            printf("\nDisconnected from server.");
            exit(0);
        }

        buffer[recv_size]='\0';

        printf("\n%s\n> ",buffer);
        fflush(stdout);

    }
}

int main(){
    system("color 0A");

    WSADATA wsa;
    WSAStartup(MAKEWORD(2,2), &wsa);

    sock = socket(AF_INET, SOCK_STREAM, 0);

    struct sockaddr_in server;
    char ip[50];

    printf("Enter server IP: ");
    scanf("%s",ip);

    server.sin_family = AF_INET;
    server.sin_port = htons(5000);
    server.sin_addr.s_addr = inet_addr(ip);

    if(connect(sock,(struct sockaddr*)&server,sizeof(server))<0){
        printf("Connection failed");
        return 1;
    }

    printf("Enter your name: ");
    scanf("%s",name);

    send(sock,name,strlen(name),0);

    CreateThread(NULL,0,receiveMessages,NULL,0,NULL);

    char message[1024];

    while(1){
        printf("> ");
        scanf(" %[^\n]", message);

        char finalmsg[1100];
        sprintf(finalmsg,"%s: %s",name,message);

        send(sock,finalmsg,strlen(finalmsg),0);
    }
}
    