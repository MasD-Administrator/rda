#include "stdio.h"
#include "winsock2.h"
#include "ws2tcpip.h"
#include <cstring>

int main(){
    int port = 5000;
    WSADATA wsa;
    if (WSAStartup(MAKEWORD(2,2), &wsa) != 0){
        printf("startup failed");
        return 1;
    }

    SOCKET listener = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (listener == INVALID_SOCKET){
        printf("invalid socket : %d\n", WSAGetLastError());
        WSACleanup();
        return 1;
    }

    sockaddr_in addr = {};
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = INADDR_ANY;
    addr.sin_port = htons(port);

    if (bind(listener, (sockaddr*) &addr, sizeof(addr)) == SOCKET_ERROR){
        printf("bind failed : %d\n", WSAGetLastError());
        closesocket(listener);
        WSACleanup();
        return 1;
    }

    if (listen(listener, SOMAXCONN) == SOCKET_ERROR){
        printf("listen failed : %d\n", WSAGetLastError());
        closesocket(listener);
        WSACleanup();
        return 1;
    }

    printf("waiting for connection in socket %d\n", port);


    SOCKET client = accept(listener, nullptr, nullptr);
    if (client == INVALID_SOCKET){
        printf("client not valid : %s\n", WSAGetLastError());
    }
    else {
        printf("client connected succesfully\n");
        char buffer[512];
        int n = recv(client, buffer, sizeof(buffer) - 1, 0);
        buffer[n] = '\0';
        printf("client said : %s", buffer);
        const char* msg2 = "hello there client";
        send(client, msg2, (int)std::strlen(msg2), 0);
    }

    closesocket(listener);
    WSACleanup();
    return 0;


}