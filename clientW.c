#include <stdio.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <string.h>

#define MAX_MESSAGE_LENGTH 1024

int main(int argc, char *argv[]) {
  printf("Début programme\n");

  // create a TCP/IP socket
  int dS = socket(PF_INET, SOCK_STREAM, 0);
  printf("Socket Créé\n");

  // initialize a sockaddr_in structure with the server's IP address and port number
  struct sockaddr_in aS;
  aS.sin_family = AF_INET;
  inet_pton(AF_INET,argv[1],&(aS.sin_addr));
  aS.sin_port = htons(atoi(argv[2]));

  socklen_t lgA = sizeof(struct sockaddr_in);
  connect(dS, (struct sockaddr *) &aS, lgA);
  printf("Socket Connecté\n");

  char message[MAX_MESSAGE_LENGTH];
  int totalBytesSent = 0;

  // send the first message
  printf("Entrez le premier message : ");
  fgets(message, MAX_MESSAGE_LENGTH, stdin);
  int messageLength = strlen(message) - 1; // exclude the newline character
  int bytesSent = send(dS, message, messageLength, 0);
  totalBytesSent += bytesSent;
  printf("Nombre d'octets envoyés pour le premier message : %d\n", bytesSent);

  // send the second message
  printf("Entrez le deuxième message : ");
  fgets(message, MAX_MESSAGE_LENGTH, stdin);
  messageLength = strlen(message) - 1;
  bytesSent = send(dS, message, messageLength, 0);
  totalBytesSent += bytesSent;
  printf("Nombre d'octets envoyés pour le deuxième message : %d\n", bytesSent);

  printf("Nombre total d'octets envoyés : %d\n", totalBytesSent);

  int r;
  recv(dS, &r, sizeof(int), 0);
  printf("Réponse reçue : %d\n", r);

  shutdown(dS, 2);
  printf("Fin du programme");
}