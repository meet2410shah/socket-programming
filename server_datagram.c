/*
  Title   : Threads and Sockets : UDP Server
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

#define MAXIMUM_CLIENT 20
#define MAXIMUM_SIZE 1024

int broadcast = 0;

char** message[MAXIMUM_SIZE][256];
int client_list[MAXIMUM_CLIENT];

// Total Number of Active Clients
int counter = 0;

// Utiliy Function
int func(int client_port) {
	for(int i = 0; i < counter; i++) {
		if(client_list[i] == client_port) {
			return i;
		}
	}
	return -1;
}

// Main Function
int main(int argc, char* argv[]) { 
	// Check if sufficient arguments
  if(argc < 3) {
    printf("Less arguments.\n$./[filename].o [PORT] [TYPE]\n");
    exit(-1);
  }

  // Check PORT number for server.
  int PORT = atoi(argv[1]);
  if(PORT == 0) {
    printf("Please give the valid PORT number arguments.\n");
    exit(-1);
  }

  // Check Type of Communication.
  if(!strcmp(argv[2], "OO")) {
    broadcast = 0;
  } else if(!strcmp(argv[2], "BC")){
    broadcast = 1;
  } else {
    printf("Please give the valid third arguments:\n1) OO - One to One or \n2) BC - Broad-Cast\n");
    exit(-1);
  }

	// Socket Variables
	int sockfd_server, n;
	char buffer[MAXIMUM_SIZE];
	struct sockaddr_in server, client;
	socklen_t sockaddr_in_size = sizeof(struct sockaddr_in);

	// Creation of Socket File Discriptor
	sockfd_server = socket(AF_INET, SOCK_DGRAM, 0);

	// Check if Error in Creation
	if(sockfd_server < 0) {
		perror("ERROR: Unable to create socket file discriptor");
		exit(-1);
	}
	
	// Preparation of the sockaddr_in structure
	memset(&server, 0, sizeof(server));
	memset(&client, 0, sizeof(client));
	server.sin_family = AF_INET;
	server.sin_addr.s_addr = INADDR_ANY;
	server.sin_port = htons(PORT);
	if(bind(sockfd_server, (const struct sockaddr *)&server, sizeof(server)) < 0) {
		perror("Bind failed");
		exit(EXIT_FAILURE);
	}
	
	bzero(buffer, MAXIMUM_SIZE);
	if(broadcast) {
		puts("UDP Server has Started (Broadcast Mode)");
	} else {
		puts("UDP Server has Started (One-to-One Mode)");
	}
	
	while(n = recvfrom(sockfd_server, (char *)buffer, MAXIMUM_SIZE, MSG_WAITALL, (struct sockaddr *) &client, &sockaddr_in_size)) {
		if(n < -1) {
			break;
		}
		int id = func(client.sin_port);
		if(id == -1) {
			client_list[counter] = client.sin_port;
			
			// Acknowledgment Message
			char initial_message[] = "Connection Successful!!!, Welcome, client A\n";
			initial_message[strlen(initial_message) - 2] += counter;
			
			puts("New Client Added");

			id = counter;
			counter++;
			sendto(sockfd_server,(char *)initial_message, strlen(initial_message), MSG_CONFIRM, (const struct sockaddr *) &client, sockaddr_in_size);
		} else {
			if(broadcast) {
				// BROADCAST MODE
				for(int i = 0; i < counter; i++) {
					if(i != id) {
						client.sin_port = client_list[i];
						sendto(sockfd_server, (const char *)buffer, 256, MSG_CONFIRM, (const struct sockaddr *) &client, sockaddr_in_size);
					}
				}
			} else {
				// ONE-TO-ONE MODE
				client.sin_port = client_list[(id + 1) % counter];
				sendto(sockfd_server, (const char *)buffer, 256, MSG_CONFIRM, (const struct sockaddr *) &client, sockaddr_in_size); 
			}
		}
		bzero(buffer, MAXIMUM_SIZE);
	}
	return 0;
}