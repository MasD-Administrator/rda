#include "stdio.h"
#include "winsock2.h"
#include "ws2tcpip.h"
#include <string>

#include <unordered_map>
#include <thread>

#include "../protocols.cpp"

bool send_all(SOCKET socket, char* data, int len){
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

// TODO: might need mutex threads
void handle_client(SOCKET client, std::unordered_map<std::string, SOCKET>* clients){
    bool connected = true;
    char buffer[PROTOCOL_LENGTH];
    int n = recv_all(client, buffer, sizeof(buffer));
    std::string user_id = buffer;
    printf("client %s connected succesfully\n", user_id.c_str());
    (*clients)[user_id] = client;

    while (connected == true){
        int n = recv_all(client, buffer, sizeof(buffer));
        std::string protocol = buffer;
        printf("protocol : %s", protocol);
        if (protocol == END){
            printf("client %s disconnected", user_id);
            (*clients).erase(user_id);
            closesocket(client);
            connected = false;
            break;
        }

        else if (protocol == MESSAGE) {
            int n = recv_all(client, buffer, sizeof(buffer));
            printf("the message sent is %s bytes long", buffer);
            char buffer[n];
            int m = recv_all(client, buffer, n);
            printf("client %s said : %s", user_id, buffer);
        }
        }
}

int main(){
    int port = 5000;
    std::unordered_map<std::string, SOCKET> clients = {}; 


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

    while (true){
        SOCKET client = accept(listener, nullptr, nullptr);
        if (client == INVALID_SOCKET){
            printf("client not valid : %s\n", WSAGetLastError());
        }
        else {
            std::thread(handle_client, client, &clients).detach();
        }
    }
    
    closesocket(listener);
    WSACleanup();
    return 0;
}
