#include "MySocket.h"
#include <stdexcept>
#include <ws2tcpip.h>
#include <winsock2.h>
#include <iostream>

#define SOCKET_TERMINATE_CHAR '%'

// ked 1 strana posle 2. strane spravu ":end", tak chce ukoncit komunikaciu
// specializovana sprava
const char* MySocket::endMessage = ":end";  // upravit podla potreby

MySocket* MySocket::createConnection(std::string hostName, short port) {
    WSADATA wsaData;
    struct addrinfo *result = NULL;
    struct addrinfo hints;
    int iResult;

    // DLL -> pripojim sa pocas behu programu

    // Initialize Winsock
    // pri praci so socketmi vo Windowse musim explicitne zavolat WSAStartup
    // "inicializujem" Windows kniznicu pre pracu so socketmi
    iResult = WSAStartup(MAKEWORD(2,2), &wsaData);  // STARTUP
    if (iResult != 0) {
        throw std::runtime_error("WSAStartup failed with error: " + std::to_string(iResult) + "\n");
    }

    ZeroMemory(&hints, sizeof(hints));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;

    // Resolve the server address and port
    // === GETADDRINFO ===
    // informacie o serveri, na ktory sa chcem pripojit
    iResult = getaddrinfo(hostName.c_str(), std::to_string(port).c_str(), &hints, &result);
    if (iResult != 0) {
        // pre kazde volanie WSAStartup musim zavolat WSACleanup
        WSACleanup();   // CLEANUP
        throw std::runtime_error("getaddrinfo failed with error: " + std::to_string(iResult) + "\n");
    }

    // Create a SOCKET for connecting to server
    // === SOCKET ===
    // vytvaram socket
    SOCKET connectSocket = socket(result->ai_family, result->ai_socktype, result->ai_protocol);
    if (connectSocket == INVALID_SOCKET) {  // ak funkcia socket vrati nevalidnu hodnotu (socket nebol uspesne vytvoreny)
        WSACleanup();
        throw std::runtime_error("socket failed with error: " + std::to_string(WSAGetLastError()) + "\n");
    }

    // Connect to server
    // === CONNECT ===
    // pripojenie na server pomocou vytvoreneho socketu
    iResult = connect(connectSocket, result->ai_addr, (int)result->ai_addrlen);
    if (iResult == SOCKET_ERROR) {
        closesocket(connectSocket);
        connectSocket = INVALID_SOCKET;
    }

    freeaddrinfo(result);

    if (connectSocket == INVALID_SOCKET) {
        printf("Unable to connect to server!\n");
        WSACleanup();
        throw std::runtime_error("Unable to connect to server.\n");
    }

    // vytvorim objekt typu MySocket
    // param: vytvoreny a pripojeny socket (zabaleny do MySocket)
    return new MySocket(connectSocket);
}

MySocket::MySocket(SOCKET socket) :
        connectSocket(socket) {
    // ziskavanie dat zo strany servera
}

MySocket::~MySocket() {
    if (this->connectSocket != INVALID_SOCKET) {
        // uzavriem socket
        closesocket(this->connectSocket);
        this->connectSocket = INVALID_SOCKET;
    }
    WSACleanup();   // vycisti po sebe
}

// ked klient len posiela data (retazec)
void MySocket::sendData(const std::string &data) {
    // uprava retazca, ktory chcem poslat cez socket
    // pretoze nemusi platit, ze jeden write = jeden read
    size_t data_length = data.length();
    char* buffer = (char*)calloc(data_length + 1, sizeof(char));
    memcpy(buffer, data.c_str(), data_length);
    // za kazdou odoslanou spravou poslem ukoncovaci znak
    buffer[data_length] = SOCKET_TERMINATE_CHAR;

    // !! SEND !!
    // poslem na socket svoj buffer, ktory ma data_length + 1 bajtov
    int iResult = send(connectSocket, buffer, data_length + 1, 0 );
    if (iResult == SOCKET_ERROR) {
        throw std::runtime_error("send failed with error: " + std::to_string(WSAGetLastError()) + "\n");
    }
    free(buffer);
    buffer = NULL;
}

// potom dorobit, aby klient mohol aj prijimat data

void MySocket::sendEndMessage() {
    // poslat ukoncovaciu spravu!
    // chcem uplne ukoncit komunikaciu, uz som poslala vsetky spravy (data), ktore som chcela poslat
    this->sendData(this->endMessage);
}

// Receive message order:
// 0: board
// 1: player on turn / game over message + winner id
// 2: board messages
// 3: player id
std::vector<std::string>* MySocket::receiveData(int numOfMessagesToWaitFor) {
    const int buffLen = 4096;
    char buffer [buffLen];
    memset(buffer, '\0', buffLen);

    std::vector<std::string>* messages = new std::vector<std::string>();
    int msgCount = 0;

    while (msgCount < numOfMessagesToWaitFor) {
        std::string receivedMsg = "";
        while (receivedMsg.find(SOCKET_TERMINATE_CHAR) == std::string::npos) {
            recv(connectSocket, buffer, buffLen, 0);
            receivedMsg = buffer;
            // generally, the client will receive message from server at the beginning of the game
            // and then only after each turn, which may take several seconds to complete
            // therefore we can sleep for a few seconds instead of checking the condition over and over again
            std::this_thread::sleep_for(std::chrono::seconds(2));
        }

        int index = 0;
        while ((index = receivedMsg.find(SOCKET_TERMINATE_CHAR)) != std::string::npos) {
            messages->push_back(receivedMsg.substr(0, index));
            receivedMsg = receivedMsg.substr(index + 1);
            msgCount++;
        }
    }

    return messages;
}

// sprava moze prist rozkuskovana na viacero sprav (dokopy ten isty obsah), resp viac sprav moze prist ako jedna velka => pocet sprav NIE je garantovany

#undef SOCKET_TERMINATE_CHAR