#include "winsock.h"
#include "ws2tcpip.h"

#include "cstring"
#include "stdio.h"


int main(){
    int port = 5000;
    WSADATA wsa;
    if (WSAStartup(MAKEWORD(2,2), &wsa) != 0){
        printf("startup failed\n");
        return 1;
    }

    SOCKET sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (sock == INVALID_SOCKET){
        printf("invalid socket");
        WSACleanup();
        return 1;
    }

    sockaddr_in addr = {};
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    inet_pton(AF_INET, "127.0.0.1", &addr.sin_addr);

    if (connect(sock, (sockaddr*) &addr, sizeof(addr)) == SOCKET_ERROR){
        printf("connection failed\n");
        closesocket(sock);
        WSACleanup();
        return 1;
    }

    printf("connected successfully!\n");

    const char* msg = "hello there server";
    send(sock, msg, (int)std::strlen(msg), 0);

    // now wait
    char buffer[512];
    int n = recv(sock, buffer, sizeof(buffer) - 1 , 0);
    if (n > 0){
        buffer[n] = '\0';
        printf("server says : %s\n", buffer);

    }



}