#pragma once
#pragma comment(lib, "ws2_32.lib")

#include <winsock2.h>
#include <stdexcept>
#include <ws2tcpip.h>
#include <string>

class MySocket {
public:
    static MySocket* createConnection(std::string hostName, short port);

    ~MySocket();
    void sendData(const std::string& data);
    void sendEndMessage();
protected:
    MySocket(SOCKET socket);
private:
    static const char* endMessage;
    SOCKET connectSocket;
};

