CCOMP = g++
SCOMP = g++

C_SOURCES = ./Client/ClientGame.cpp ./Client/ClientGame.cpp ./Client/MySocket.cpp
S_SOURCES = ./Server/ServerMain.cpp ./Server/Board.cpp ./Server/Player.cpp ./Server/Square.cpp ./Server/ServerConnection.cpp ./Server/Game.cpp
C_OBJECTS = $(C_SOURCES:.cpp=.o)
S_OBJECTS = $(S_SOURCES:.cpp=.o)

all: client server

client:	$(C_OBJECTS)
	$(CCOMP) $(C_SOURCES) -o outClient

server:	$(S_OBJECTS)
	$(SCOMP) $(S_SOURCES) -o outServer

clean:
	rm -f client server $(C_OBJECTS) $(S_OBJECTS) outClient out Server