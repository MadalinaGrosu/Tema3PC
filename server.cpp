#include "helpers.h"

// ./server port_server
int main(int argc, char *argv[]) {
	time_t current_time; // timpul curent
	std::list<client> clients; // lista cu toti clientii conectati
	int sockfd, port_server;
	struct sockaddr_in serv_addr, cli_addr;
	
	if (argc != 2) {
		printf("Usage: %s port_server\n",argv[0]);
		return -1;
	}
	
	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	    if (sockfd < 0) 
	       perror("ERROR opening socket");
	        
	port_server = atoi(argv[1]);
	
	memset((char *) &serv_addr, 0, sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = INADDR_ANY;	// foloseste adresa IP a masinii
	serv_addr.sin_port = htons(port_server);
	     
	if (bind(sockfd, (struct sockaddr *) &serv_addr, sizeof(struct sockaddr)) < 0) 
		perror("ERROR on binding");
	
}
