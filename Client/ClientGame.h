#pragma once

#include <iostream>
#include <string>
#include <stdexcept>
#include <thread>

#include "MySocket.h"

namespace Client
{
	class ClientGame
	{
	private:
		MySocket* socket;
        int playerId = 0;
        int turn = 1;
        bool gameOver = false;

	public:
		void Connect(std::string ipaddr, unsigned int port);
        void Play();
        int Throw();
        int PickPawn();
        void YouAreColor(int id);
		~ClientGame();
	};
}

