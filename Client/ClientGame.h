#pragma once

#include <iostream>
#include <string>
#include <stdexcept>
#include <thread>
#include <random>

#include "MySocket.h"

#define RESET   "\033[0m"
#define RED     "\033[31m"
#define GREEN   "\033[32m"
#define YELLOW  "\033[33m"
#define BLUE    "\033[34m"

namespace Client
{
	class ClientGame
	{
	private:
		MySocket* socket;
        int playerId = 0;
        int turn = 1;
        bool gameOver = false;
        std::random_device rd;
        std::mt19937 gen{rd()};
        std::uniform_int_distribution<> distrib{1, 6};
        bool disconnected = false;

	public:
        const std::string GAME_OVER = "GAME_OVER";

		void Connect(std::string ipaddr, unsigned int port);
        void Play();
        int ThrowDice();
        int PickPawn();
        void YouAreColor(int id);
		~ClientGame();
	};
}

