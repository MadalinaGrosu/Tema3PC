#include "helpers.h"
#include <list>
#include <stack>
#include <sstream> 
#include <queue>

// ./client nume_client port_client ip_server port_server
int main (int argc, char* argv[]) {
	int port_client, port_server, sockfd, fdmax,n;	
	std::stack<msg> histoy;
	struct sockaddr_in serv_addr;
	char payload[MAXLEN], cmd[25];
	msg s;
	fd_set read_fds;
    fd_set tmp_fds;
    std::queue< std::pair<char*,char*> > outbox;
	
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
    
    while(1){
		tmp_fds = read_fds;
		if (select(fdmax + 1, &tmp_fds,NULL,NULL,NULL) == -1) {
			perror("Error in select\n");
		}
		
		if (FD_ISSET(sockfd,&tmp_fds)) {
				//citesc date de pe socket
				memset(&s,0,sizeof(msg));
				n = recv(sockfd,&s,sizeof(msg),0);
				
				if (s.type == TYPE8) {
					printf("%s: I'm out!\n",argv[1]);
					return 0;
				}
				
				if (s.type == TYPE7) {
					printf("%s: I quit!\n",argv[1]);
					return 0;
				}
				
				if (s.type == TYPE1) {
					
					char *pch = s.payload;
					printf("There are %i clients connected: \n",s.len);
					pch = strtok(s.payload," ");
					while (pch != NULL) {
						printf("%s\n",pch);
						pch = strtok(NULL," ");
					}
						
				}
				
				if (s.type == TYPE2) {
					if (s.len == 0) {
						char *pch = strtok(s.payload, " ");
						printf("Client %s ",pch);
						pch = strtok(NULL," ");
						printf("has the ip %s, ",pch);
						pch = strtok(NULL, " ");
						printf("listens on port %s, ", pch);
						pch = strtok(NULL, " ");
						printf("and it's online from %s s\n", pch);
					} 
					if (s.len == 1) {
						printf("Client %s it is not online!\n",s.payload);
					}
				}
		}
		
		if (FD_ISSET(0,&tmp_fds)) {
			printf(">");
			fgets(cmd,MAXLEN,stdin);
			printf("%s\n",cmd);
			char *p = cmd;
			p = strtok(cmd," ");
			printf("%s\n",p);
			if (strstr(p,"listclients")) {
				memset(&s,0,sizeof(msg));
				s.type = TYPE1;
				send(sockfd,&s,sizeof(s),0);
			}
			
			if (strstr(p,"infoclient")) {
				p = strtok(NULL," ");
				memset(&s,0,sizeof(msg));
				s.type = TYPE2;
				strncpy(s.payload,p,strlen(p) - 1);
				send(sockfd,&s,sizeof(s),0);
			}
			
			if (strncmp(p,"message",strlen(p)) == 0) {
				p = strtok(NULL," ");
				memset(&s,0,sizeof(msg));
				s.type = TYPE3;
				strncpy(s.payload,p,strlen(p) - 1);
				
				p = strtok(NULL," ");
				outbox.push(std::pair<char*,char*>(s.payload,p));
				send(sockfd,&s,sizeof(s),0);				
			}
		}
	}
			
    
}
