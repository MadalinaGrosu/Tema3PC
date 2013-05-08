#ifndef _HELPERS_H
#define _HELPERS_H 1
#include <sys/socket.h>
#include <netinet/in.h>
#include <time.h>
#include <stdio.h>
#include <sys/types.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#include <netdb.h> 
#include <unistd.h>




#define MAXLEN 1024
#define MAXCLIENTS 10

#define TYPE0 0 // hello
#define TYPE1 1 // list clients
#define TYPE2 2 // info client
#define TYPE3 3 // message
#define TYPE4 4 // broadcast 
#define TYPE5 5 // sendfile
#define TYPE6 6 // history
#define TYPE7 7 // quit
#define TYPE8 8 // disconnect

typedef struct {
	int type;
	int len;
	char payload[MAXLEN];
} msg;

typedef struct {
	char name[255];
	int port;
	time_t time;
	int sockfd;
} client;

#endif
