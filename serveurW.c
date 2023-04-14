#include <stdio.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <string.h>

#define MAX_BYTES 124

int main(int argc, char *argv[]) {
  
  printf("Début programme\n");

  // create a TCP/IP socket
  int dS = socket(PF_INET, SOCK_STREAM, 0);
  printf("Socket Créé\n");

  // initialize a sockaddr_in structure with the local IP address and port number
  struct sockaddr_in ad;
  ad.sin_family = AF_INET;
  ad.sin_addr.s_addr = INADDR_ANY ;
  ad.sin_port = htons(atoi(argv[1])) ;

  // assign the socket to the local IP address and port number using the sockaddr_in structure
  bind(dS, (struct sockaddr*)&ad, sizeof(ad)) ;
  printf("Socket Nommé\n");

  // set the socket to listen for incoming connections
  listen(dS, 1) ;
  printf("Mode écoute\n");

  // accept an incoming connection and get the client's address information
  struct sockaddr_in aC ;
  socklen_t lg = sizeof(struct sockaddr_in) ;
  int dSC = accept(dS, (struct sockaddr*) &aC,&lg) ;
  printf("Client Connecté\n");

  char msg[MAX_BYTES];
  // receive a message from the client using the accepted socket
  int bytes_received = recv(dSC, msg, MAX_BYTES, 0);
  printf("Nombre d'octets reçus : %d\n", bytes_received); // prints the number of received bytes to the console

  // close the accepted socket and the listening socket
  shutdown(dSC, 2) ; 
  shutdown(dS, 2) ;
  printf("Fin du programme\n");
}






