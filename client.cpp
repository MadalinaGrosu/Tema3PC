#include "helpers.h"
#include <list>
#include <stack>

// ./client nume_client port_client ip_server port_server
int main (int argc, char* argv[]) {
	int port_client, port_server, sockfd;	
	std::stack<msg> histoy;
	struct sockaddr_in serv_addr;
	
	sockfd = socket(AF_INET,SOCK_STREAM,0);
	if (sockfd < 0) {
		error("Error opening socket");
	}
	
}
