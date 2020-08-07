/*
  Title   : Threads and Sockets : TCP Client
  Author  : Meet Shah
  ID      : 201801434
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>

#define MAXIMUM_SIZE 1024

// Main Function
int main(int argc, char *argv[]) {
  // Check if sufficient arguments
  if(argc < 3) {
    printf("Less arguments.\n$./[filename].o [ADDRESS] [PORT]\n");
    exit(-1);
  }

  // SERVER ADDRESS
  struct hostent *server_host;
  server_host = gethostbyname(argv[1]);

  // Check PORT number for server.
  int PORT = atoi(argv[2]);
  if(PORT == 0) {
    printf("Please give the valid PORT number arguments.\n");
    exit(-1);
  }

  // Socket Variables
  int sockfd_client, n;
  sockfd_client = socket(AF_INET, SOCK_STREAM, 0);
  struct sockaddr_in server;
  
  // Check if Error in Creation
  if(sockfd_client < 0) {
    perror("ERROR opening socket");
    exit(0);
  }
  
  if(server_host == NULL) {
    fprintf(stderr,"ERROR, no such host\n");
    exit(0);
  }
    
  bzero((char *) &server, sizeof(server));
  server.sin_family = AF_INET;
  bcopy((char *)server_host->h_addr,
  (char *)&server.sin_addr.s_addr, 
  server_host->h_length);
  
  server.sin_port = htons(PORT);

  if(connect(sockfd_client,(struct sockaddr *)&server,sizeof(server)) < 0) {
    perror("ERROR connecting");
    exit(-1);
  }
  
  char buffer[256];
  puts("You are able to send message to other client");

  n = recv(sockfd_client, buffer, 255, 0);
  printf("%s\n", buffer);
  int pid = fork();
  if(pid == 0) {
	  bzero(buffer, 256);
    while(n = recv(sockfd_client, buffer, 255, 0)) {
      if(n < 0) {
        perror("ERROR reading from socket");
        exit(-1);
      }
      if(n > 0) {
        printf("Server Response: %s\n", buffer);
      }
    }
  } else {
	  bzero(buffer, 256);
	  while(1) {
      scanf("%[^\n]%*c", buffer);
      n = send(sockfd_client, buffer, strlen(buffer) + 1, 0);
      if(n < 0) {
        perror("ERROR reading from socket");
        exit(-1);
      }
  	}
  }
  return 0;
}
