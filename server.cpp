#include "helpers.h"
#include <list>
#include <sstream> 

int containsClient(std::list<client> clients, char* name) {
	for (std::list<client>::iterator it = clients.begin(); it != clients.end(); ++it) {
		if (strcmp((*it).name,name) == 0)
			return 0;
	}
	
	return 1;
}

client getClient(std::list<client> clients, char* name) {
	for (std::list<client>::iterator it = clients.begin(); it != clients.end(); ++it) {
		if (strncmp((*it).name,name,strlen(name) - 1) == 0) {
			return *it;
		}
	}
	
	return *clients.end();
}

// ./server port_server
int main(int argc, char *argv[]) {
	time_t current_time; // timpul curent
	std::list<client> clients; // lista cu toti clientii conectati
	int sockfd, port_server,i, newsockfd, n;
	socklen_t clilen;
	struct sockaddr_in serv_addr, cli_addr;
	fd_set read_fds;	//multimea de citire folosita in select()
	fd_set tmp_fds;	//multime folosita temporar 
	int fdmax;		//valoare maxima file descriptor din multimea read_fds
	msg buffer;
	client cli;
	char payload[MAXLEN];
	
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
		
	FD_ZERO(&read_fds);
	FD_ZERO(&tmp_fds);
	
	listen(sockfd, MAXCLIENTS);
	
	//adaugam noul file descriptor (socketul pe care se asculta conexiuni) in multimea read_fds
	FD_SET(sockfd, &read_fds);
	fdmax = sockfd;
	FD_SET(0, &read_fds);
	
	while (1) {
			tmp_fds = read_fds; 
			if (select(fdmax + 1, &tmp_fds, NULL, NULL, NULL) == -1) 
				perror("ERROR in select");
			
			for(i = 0; i <= fdmax; i++) {
				if (FD_ISSET(i, &tmp_fds)) {
				
					if (i == sockfd) {
						// a venit ceva pe socketul inactiv(cel cu listen) = o noua conexiune
						// actiunea serverului: accept()
						clilen = sizeof(cli_addr);
						if ((newsockfd = accept(sockfd, (struct sockaddr *)&cli_addr, &clilen)) == -1) {
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
					}
					else {
						// am primit date pe unul din socketii cu care vorbesc cu clientii
						//actiunea serverului: recv()
						if ((n = recv(i, &buffer, sizeof(buffer), 0)) <= 0) {
							if (n == 0) {
								//conexiunea s-a inchis
								printf("selectserver: socket %d hung up\n", i);
							} else {
								perror("ERROR in recv");
							}
							close(i); 
							FD_CLR(i, &read_fds); // scoatem din multimea de citire socketul pe care 
						} 
						else { //recv intoarce >0
							switch (buffer.type) {
								case TYPE0 :
								{
									if (containsClient(clients,buffer.payload) == 0) {
										// send disconnect message
										memset(&buffer,0,sizeof(msg));
										buffer.type = TYPE8;
										send(i,&buffer,sizeof(buffer),0);
										break;
									}
									printf("Nume client: %s\t Port: %i\n",buffer.payload,buffer.len);
									memset(&cli,0,sizeof(client));
									strcpy(cli.name,buffer.payload);
									cli.port = buffer.len;
									cli.sockfd = i;
									time(&cli.time);
									clients.push_front(cli);
									break;
								}
								case TYPE1 :
								{
									// send list of clients
									memset(&buffer,0,sizeof(msg));
									buffer.type = TYPE1;
									buffer.len = clients.size();
									
									std::stringstream buff;
									for (std::list<client>::iterator it = clients.begin(); it != clients.end(); ++it) {
										buff << (*it).name << " ";
									}
									
									std::string mesaj = buff.str();
									memcpy(buffer.payload,mesaj.c_str(),mesaj.size());
									send(i, &buffer, sizeof(msg), 0);
									break;
								}
								case TYPE2 :
								{
									// send info client
									//printf("Buffer: %s\n",buffer.payload);
									cli = getClient(clients,buffer.payload);
	
									
									if (strncmp(cli.name,buffer.payload,strlen(buffer.payload) - 1) == 0) {										
										memset(&buffer,0,sizeof(msg));
										buffer.type = TYPE2;
										buffer.len = 0;
										
										time(&current_time);
										current_time -= cli.time;
										
										// trimit datele despre client in formatul
										// port timp_scurs
										std::stringstream buff;
										buff << cli.name << " " << cli.port << " " << current_time;
										
										std::string mesaj = buff.str();
										memcpy(buffer.payload,mesaj.c_str(),mesaj.size());
																
									} else {
										buffer.type = TYPE2;
										buffer.len = 1;
									}
									send(i,&buffer, sizeof(msg), 0);
									break;
								}
							}
					}
				}
			}
		}
	}
}

