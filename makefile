CC = g++

.PHONY: cleanClient cleanServer none

none:
	echo "Need to specify client/server explicitly (win/linux) i.e. 'make client'/'make server'"

client: ./Client/ClientGame.cpp ./Client/ClientGame.h ./Client/ClientMain.cpp ./Client/MySocket.cpp ./Client/MySocket.h
	$(CC) -o client ./Client/ClientGame.cpp ./Client/ClientMain.cpp ./Client/MySocket.cpp

server: ./Server/Board.cpp ./Server/Board.h ./Server/Game.cpp ./Server/Game.h ./Server/Player.cpp ./Server/Player.h ./Server/ServerConnection.cpp ./Server/ServerConnection.h ./Server/ServerMain.cpp ./Server/Square.cpp ./Server/Square.h
	$(CC) -o server ./Server/Board.cpp ./Server/Game.cpp ./Server/Player.cpp ./Server/ServerConnection.cpp ./Server/ServerMain.cpp ./Server/Square.cpp

cleanClient:
	del client

cleanServer:
	rm -f server
