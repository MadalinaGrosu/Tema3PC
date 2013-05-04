#include "helpers.h"
#include <list>
#include <stack>

// ./client nume_client port_client ip_server port_server
int main (int argc, char* argv[]) {
	int port_client, port_server, sockfd, fdmax;	
	std::stack<msg> histoy;
	struct sockaddr_in serv_addr;
	char payload[MAXLEN];
	msg s;
	fd_set read_fds;
    fd_set tmp_fds;
	
	if (argc != 5) {
		printf("Usage: %s nume_client port_client ip_server port_server\n",argv[0]);
		return -1;
	}
	port_client = atoi(argv[2]);
	port_server = atoi(argv[4]);
	
	// creez un nou socket
	sockfd = socket(AF_INET,SOCK_STREAM,0);
	if (sockfd < 0) {
		perror("Error opening socket");
	}

	// connect la server
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_port = htons(port_server);
	inet_aton(argv[3], &serv_addr.sin_addr);
	
	if (connect(sockfd,(struct sockaddr*) &serv_addr,sizeof(serv_addr)) < 0) 
        perror("ERROR connecting");   
    
    FD_ZERO(&read_fds);
    FD_ZERO(&tmp_fds);
    FD_SET(sockfd,&read_fds);
    fdmax = sockfd;
    FD_SET(0,&read_fds);
    
    // trimit un mesaj de hello
    s.type = TYPE0;
    memset(s.payload,0,MAXLEN);
    memcpy(s.payload,argv[1], strlen(argv[1]));
   // printf("nume: %s",s.payload);
    s.len = port_client;
    
    send(sockfd,&s,sizeof(s),0);
    
}
