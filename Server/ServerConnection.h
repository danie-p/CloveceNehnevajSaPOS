//
// Created by Kristian on 3. 1. 2024.
//

#ifndef SERVER_SERVERCONNECTION_H
#define SERVER_SERVERCONNECTION_H

#include <iostream>
#include <vector>

#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>

#include "Game.h"

//#include <stdio.h>
//#include <stdlib.h>
//#include <stdbool.h>
//#include <string.h>
//#include <strings.h>
//#include <pthread.h>
//#include <sys/socket.h>
//#include <arpa/inet.h>
//#include <unistd.h>
//#include "buffer.h"
//#include "char_buffer.h"
//#include "active_socket.h"
//#include "passive_socket.h"

namespace Server {

    const int PLAYER_COUNT = 4;

    void Create(int port);
    void PlayGame(int clientSockets[]);
}

#endif //SERVER_SERVERCONNECTION_H
