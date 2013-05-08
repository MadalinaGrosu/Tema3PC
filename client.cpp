#include "helpers.h"
#include <list>
#include <stack>
#include <sstream> 
#include <queue>
#include <vector>

void send_message(char *ip_addr, char *port, char *mesaj, char *name) {
	struct sockaddr_in client_addr;
	msg s;
	
	int sockfd = socket(AF_INET,SOCK_STREAM,0);
	if (sockfd < 0) {
		perror("Error opening socket");
	}
	
	//connect to client
	client_addr.sin_family = AF_INET;
	client_addr.sin_port = htons(atoi(port));
	inet_aton(ip_addr,&client_addr.sin_addr);
	
	if (connect(sockfd,(struct sockaddr*) &client_addr,sizeof(client_addr)) < 0) 
        perror("ERROR connecting");   
        
    s.type = TYPE3;
    strcpy(s.payload,name);
    s.payload[strlen(name)] = ' ';
    strncpy(s.payload + strlen(name)+1, mesaj, strlen(mesaj));
  //printf("Payload: %s\n", s.payload);
  //printf("Sockfd %i\n", sockfd);
    send(sockfd,&s,sizeof(s),0);
    
    close(sockfd);
	
}

// ./client nume_client port_client ip_server port_server
int main (int argc, char* argv[]) {
	int port_client, port_server, sockfd, fdmax,n, listen_sockfd, newsockfd;	
	std::stack<msg> histoy;
	struct sockaddr_in serv_addr, cli_addr;
	char payload[MAXLEN], cmd[25];
	
	msg s;
	fd_set read_fds;
    fd_set tmp_fds;
    socklen_t clilen;
	
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
        
    listen_sockfd = socket(AF_INET,SOCK_STREAM,0);
    if (listen_sockfd < 0) {
		perror("Error opening socket");
	}
        
    cli_addr.sin_family = AF_INET;
    cli_addr.sin_port = htons(port_client);
    cli_addr.sin_addr.s_addr = INADDR_ANY;	
    
    if (bind(listen_sockfd, (struct sockaddr *) &cli_addr, sizeof(struct sockaddr)) < 0) 
		perror("ERROR on binding");
    
    FD_ZERO(&read_fds);
    FD_ZERO(&tmp_fds);
    
    FD_SET(listen_sockfd,&read_fds);
    FD_SET(0,&read_fds);
    fdmax = listen_sockfd;
   // printf("Listen: %i\n", listen_sockfd);
    FD_SET(sockfd,&read_fds);
    
    if (sockfd > fdmax)
		fdmax = sockfd;
  //  printf("Sockfd %i\n",sockfd);
    
    listen(listen_sockfd,MAXCLIENTS);
    
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
		
		for(int i = 0; i <= fdmax; i++) {
			if (i == sockfd && FD_ISSET(sockfd,&tmp_fds)) {
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
			} else if (i == 0 && FD_ISSET(0,&tmp_fds)) {
				printf(">");
				fgets(cmd,MAXLEN,stdin);
				printf("%s\n",cmd);
				char *p = cmd;
				p = strtok(cmd," ");
			//	printf("%s\n",p);
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
					strncpy(s.payload,p,strlen(p));
					
					p = strtok(NULL,"\n");
					char *mesaj = p;
					
					// trimit mesaj infoclient la server
					send(sockfd,&s,sizeof(s),0);		
					memset(&s,0,sizeof(msg));
					recv(sockfd,&s,sizeof(msg),0);
					if (s.len == 0) {
				//		printf("%s recieved payload: %s\n", argv[1], s.payload);
						p = strtok(s.payload," "); //nume client
						p = strtok(NULL," ");	 // ip_client
				//		printf("%s\n", p);
						char *ip = p;
				//		printf("Ip: %s\n", ip);
						p = strtok(NULL, " "); //listen_port
				//		printf("Port: %s, IP: %s\n", p, ip);
						send_message(ip, p, mesaj,argv[1]);
					} else {
						printf("Client %s is offline!\n", s.payload);
					}
				}
				
				if (strncmp(p, "broadcast", strlen(p)-1) == 0) {
					char *mesaj;
					p = strtok(NULL,"\n");
					mesaj = p;
					
					// trimit cerere listclients
					memset(&s,0,sizeof(msg));
					s.type = TYPE1;
					send(sockfd,&s,sizeof(msg),0);
					
					memset(&s,0,sizeof(msg));
					recv(sockfd,&s,sizeof(msg),0);
					
					if (s.len == 0) {
						printf("There are no clients connected!\n");
					} else {
						char *pch;
						printf("S.payload: %s\n", s.payload);
						pch = strtok(s.payload," ");
						msg r;
						std::vector<char*> clients;
						
						while (pch != NULL) {
							clients.push_back(pch);
							pch = strtok(NULL," ");
						}
						
						for(i = 0; i < clients.size(); ++i) {
							if (strcmp(argv[1],clients[i])) {
								// trimit cerere infoclient
								r.type = TYPE3;
								strcpy(r.payload,clients[i]);
								send(sockfd,&r,sizeof(msg),0);
								
								memset(&r,0,sizeof(msg));
								recv(sockfd,&r,sizeof(msg),0);
								
								printf("Payload %s\n", r.payload);
								
								if (r.len == 0) {
									p = strtok(r.payload," "); //nume client
									p = strtok(NULL," ");	 // ip_client
									char *ip = p;
									p = strtok(NULL, " "); //listen_port
									send_message(ip, p, mesaj,argv[1]);
								} else {
									printf("Client %s is offline!\n", pch);
								} 
							}
							memset(&r,0,sizeof(msg));
						}
						
					}
				}
				
				if (strncmp(p,"quit",strlen(p) - 1) == 0) {
					memset(&s,0,sizeof(msg));
					s.type = TYPE7;
					strcpy(s.payload,argv[1]);
					send(sockfd,&s,sizeof(msg),0);
					return 0;
				}
			} else if (i == listen_sockfd && FD_ISSET(i,&tmp_fds)) { 
						// a venit ceva pe socketul inactiv(cel cu listen) = o noua conexiune
						// actiunea : accept()
						clilen = sizeof(cli_addr);
						if ((newsockfd = accept(listen_sockfd, (struct sockaddr *)&cli_addr, &clilen)) == -1) {
							perror("ERROR in accept");
						} 
						else {
							//adaug noul socket intors de accept() la multimea descriptorilor de citire
							FD_SET(newsockfd, &read_fds);
							if (newsockfd > fdmax) { 
								fdmax = newsockfd;
							}
						}
						
						printf("Noua conexiune de la %s, port %d, socket_client %d\n ", inet_ntoa(cli_addr.sin_addr), ntohs(cli_addr.sin_port), newsockfd);
			} else if (FD_ISSET(i,&tmp_fds)) {
			//	printf("i %i\n", i);
				memset(&s,0,sizeof(msg));
				if ((n = recv(i, &s, sizeof(s), 0)) <= 0) {
								if (n == 0) {
									//conexiunea s-a inchis
									printf("Client %s: socket %d hung up\n", argv[1], i);
								} else {
									perror("ERROR in recv");
								}
								close(i); 
								FD_CLR(i, &read_fds); // scoatem din multimea de citire socketul pe care 
				} else {
					if (s.type == TYPE3) {
						time_t rawtime;
						struct tm * timeinfo;
						time (&rawtime);
						timeinfo = localtime (&rawtime);
						
						printf("[%2i:%2i]",timeinfo->tm_hour, timeinfo->tm_min);
						
						char *pch = strtok(s.payload," ");
						printf("[%s] ",pch);
						pch = strtok(NULL,"\0");
						printf("%s\n",pch);
					}
				} 
			}
		}
	}
			
    
}
