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
	public:
		void Connect(std::string ipaddr, unsigned int port);
        void WaitForStart();
		~ClientGame();
	};
}

