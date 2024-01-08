#ifndef SOCKETS_CLIENT_MY_SOCKET_H
#define SOCKETS_CLIENT_MY_SOCKET_H
#pragma comment(lib, "ws2_32.lib")
#include <winsock2.h>
#include <string>
#include <thread>
#include <vector>

struct GameData {
    GameData(std::vector<std::string>* dataToParse)
            : playerId(dataToParse->at(0)), board(dataToParse->at(1)),
              playerOnTurn(dataToParse->at(2)), boardMessages(dataToParse->at(3)),
              turn(dataToParse->at(4)), gameOver(dataToParse->at(5)),
              winnerColor(dataToParse->at(6)), playerOnTurnColor(dataToParse->at(7)) {}

    const std::string playerId;
    const std::string board;
    const std::string playerOnTurn;
    const std::string boardMessages;
    const std::string turn;
    const std::string gameOver;
    const std::string winnerColor;
    const std::string playerOnTurnColor;
};

class MySocket {
public:
    // umozni nadviazat spojenie so serverom
    // ako param poslem meno serveru (dns zaznam) a cislo portu
    // vytvori sa socket
    // vykona sa connect
    // vrati objekt MySocket -> nad nim uz mozem posielat data
    static MySocket* createConnection(std::string hostName, short port);
    ~MySocket();
    // posielam retazec dat (vzdy po sieti posielam len retazec!!)
    // posielam text => nemusim prevadzat na little/big endian
    void sendData(const std::string& data);

    GameData* receiveData(int numOfMessagesToWaitFor);
protected:
    MySocket(SOCKET socket);
private:
    static const char * endMessage;
    // socket, pomocou ktoreho budem komunikovat so serverom
    // alias pre typ unsigned int pointer
    SOCKET connectSocket;
};

#endif //SOCKETS_CLIENT_MY_SOCKET_H
