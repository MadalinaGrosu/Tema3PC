all: client server

client: client.cpp helpers.h
	g++ -g -Wall -o client client.cpp
server: server.cpp helpers.h
	g++ -g -Wall -o server server.cpp
