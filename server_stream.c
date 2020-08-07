/*
  Title   : Threads and Sockets : TCP Server
  Author  : Meet Shah
  ID      : 201801434
*/

/**
  USE THE FOLLOWING COMMAND TO COMPILE
  $ gcc -pthread 201801434_server_stream.c -o 201801434_server_stream.o
*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/socket.h>
#include <arpa/inet.h>

#define MAXIMUM_CLIENTS 20
#define MAXIMUM_SIZE 1024

int broadcast = 0;

char message[MAXIMUM_CLIENTS][MAXIMUM_SIZE]; 
int client_list[MAXIMUM_CLIENTS];

// Total Number of Active Clients
int counter = 0; 

// Connection Handler
void *connection_handler(void *num) {  
  int id = *(int*)num;
  int current_client = client_list[id];
  puts("New Client Added");

  // Acknowledgment Message
  char initial_message[] = "Connection Successful!!!, Welcome, client A";
  initial_message[strlen(initial_message) - 1] += id;
  send(current_client, initial_message, strlen(initial_message) + 1, 0);

  int l;
  while(l = recv(current_client, message[id], MAXIMUM_SIZE, 0) > 0) {
    if(broadcast == 1) {
      // BROADCAST MODE
      for(int j = 0; j < counter; j++) {
        if(j != id) {
          send(client_list[j], message[id], strlen(message[id]) + 1, 0);
        }
      }
    } else {
      // ONE-TO-ONE MODE
      if((id + 1) % counter == id) {
        char *alone = "You are the only client!!!";
        send(client_list[(id + 1) % counter], alone, strlen(alone) + 1, 0);
      } else {
        send(client_list[(id + 1) % counter], message[id], strlen(message[id]) + 1, 0);
      }
    }
  }	
}

// Main Function
int main(int argc, char *argv[]) {
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
  int sockfd_server, sockfd_client;
  struct sockaddr_in server, client;
  socklen_t sockaddr_in_size = sizeof(struct sockaddr_in);

  // Creation of Socket File Discriptor
  sockfd_server = socket(AF_INET, SOCK_STREAM, 0);

  // Check if Error in Creation
  if(sockfd_server < 0)	{
    printf("ERROR: Unable to create socket file discriptor");
    exit(-1);
  }
  
  // Preparation of the sockaddr_in structure
  bzero((char *) &server, sizeof(server));
  server.sin_family = AF_INET;
  server.sin_addr.s_addr = INADDR_ANY;
  server.sin_port = htons(PORT);
  
  // Bind the Socket with the Server Address
  if(bind(sockfd_server, (const struct sockaddr *) &server, sizeof(server)) < 0) {
    printf("ERROR: Unable to bind");
    return 1;
  }

  // Listen
  listen(sockfd_server, MAXIMUM_CLIENTS);
  
  // Accept Incoming Connections;
  if(broadcast) {
		puts("TCP Server has Started (Broadcast Mode)");
	} else {
		puts("TCP Server has Started (One-to-One Mode)");
	}

  while(sockfd_client = accept(sockfd_server, (struct sockaddr *)&client, (socklen_t*)&(sockaddr_in_size))) {
    // Check if Accept Error
    if(sockfd_client < 0) {
      perror("Error in Accept");
      return 1;
    }
    
    // Store Socket Descriptors of Clients
    client_list[counter] = sockfd_client;
    
    pthread_t client_thread;
    int *id = malloc(sizeof(int));
    *id = counter;
    counter++;

    if(pthread_create(&client_thread, NULL, connection_handler, (void*)id) < 0) {
      // Check if Thread Creation Error
      perror("Error in Thread Creation");
      return 1;
    }
  }
  return 0;
}
