/*
  Title   : Threads and Sockets : UDP Client
  Author  : Meet Shah
  ID      : 201801434
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h> 
#include <arpa/inet.h>
#include <netinet/in.h>

#define MAXIMUM_SIZE 1024

// Main Function
int main(int argc, char * argv[]) { 
	// Check if sufficient arguments
  if(argc < 2) {
    printf("Less arguments.\n$./[filename].o [PORT]\n");
    exit(-1);
  }
	
	// Check PORT number for server.
  int PORT = atoi(argv[1]);
  if(PORT == 0) {
    printf("Please give the valid PORT number arguments.\n");
    exit(-1);
  }

	int sockfd_client;
	struct sockaddr_in server;
	char buffer[MAXIMUM_SIZE];
	char *hello = "Hello from client";
	
	// Check if Error in Creation
	if((sockfd_client = socket(AF_INET, SOCK_DGRAM, 0)) < 0) { 
		perror("Socket creation failed");
		exit(EXIT_FAILURE);
	}

	// Preparation of the sockaddr_in structure
	memset(&server, 0, sizeof(server));
	server.sin_family = AF_INET;
	server.sin_port = htons(PORT);
	server.sin_addr.s_addr = INADDR_ANY;	
	
	int n, len; 
	char buf[MAXIMUM_SIZE] = "I am Client";
	n = sendto(sockfd_client, (const char *)buf, MAXIMUM_SIZE,  MSG_CONFIRM, (const struct sockaddr *) &server, sizeof(server));
	
	bzero(buf, sizeof(buf));
	n = recvfrom(sockfd_client, (char *)buf, MAXIMUM_SIZE, MSG_WAITALL, (struct sockaddr *) &server, &len);

	if(n > 0) printf("%s", buf);

	int pid = fork();
	if(pid == 0) {
		char bufferRecv[256];
		bzero(bufferRecv, 256);
		while(n = recvfrom(sockfd_client, (char *)bufferRecv, MAXIMUM_SIZE, MSG_WAITALL, (struct sockaddr *) &server, &len)) {
			if(n < 0) perror("ERROR reading from socket");
			if(n > 0) printf("Server Response: %s\n", bufferRecv);
		}
	} else {
		char bufferSend[256];
		bzero(bufferSend, 256);
		while(scanf("%[^\n]%*c",bufferSend)) {
			n = sendto(sockfd_client, (const char *)bufferSend, strlen(bufferSend),  MSG_CONFIRM, (const struct sockaddr *) &server, 
			sizeof(server));
			if(n < 0)  perror("ERROR reading from socket");
		}
	}
	return 0;
}