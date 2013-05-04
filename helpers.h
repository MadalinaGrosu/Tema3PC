#ifndef _HELPERS_H
#define _HELPERS_H 1
#include <sys/socket.h>
#include <netinet/in.h>
#include <time.h>
#include <stdio.h>
#include <sys/types.h>
#include <stdlib.h>
#include <string.h>



#define MAXLEN 1024
#define MAXCLIENTS 10

#define TYPE0 "hello"
#define TYPE1 "listclients"
#define TYPE2 "infoclient"
#define TYPE3 "message"
#define TYPE4 "broadcast"
#define TYPE5 "sendfile"
#define TYPE6 "history"
#define TYPE7 "quit"
#define TYPE8 "status"
#define TYPE9 "kick"

typedef struct {
	char* type;
	int len;
	char payload[MAXLEN];
} msg;

typedef struct {
	int port;
	time_t time;
	struct in_addr ip_addr;
} client;

#endif
