#include "winsock2.h"
#include "ws2tcpip.h"

#include "cstring"
#include "stdio.h"

#include "../protocols.cpp"

bool send_all(SOCKET socket, const char* data, int len){
    int sent = 0;
    while (sent < len){
        int n = send(socket, data + sent, len - sent, 0);
        if (n == SOCKET_ERROR) return false;
        sent += n;
    }
    return true;
}

bool recv_all(SOCKET socket, char* data, int len){
    int got = 0;
    while (got < len){
        int n = recv(socket, data + got, len - got, 0);
        if (n <= 0) return false;
        got += n;
    }
    return true;
}

bool send_msg(SOCKET socket, uint8_t type, const char* data, uint32_t len){
    uint32_t network_length = htonl(len);
    if (!send_all(socket, (const char*)&type, 1)) return false;
    if (!send_all(socket, (const char*)&len, sizeof(network_length))) return false;
    if (len > 0 && !send_all(socket, data, (int)len)) return false;

}


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

    char* msg = "hello there server";
    char* username = "SooriyaDeepana";

    send_all(sock, username, std::strlen(username));

    send_all(sock, MESSAGE, (int)std::strlen(MESSAGE));
    
    send_all(sock, msg, (int)std::strlen(msg));

    send_all(sock, END, (int)std::strlen(END));

    // now wait
    char buffer[PROTOCOL_LENGTH];
    int n = recv(sock, buffer, sizeof(buffer) - 1 , 0);
    if (n > 0){
        buffer[n] = '\0';
        printf("server says : %s\n", buffer);

    }



}