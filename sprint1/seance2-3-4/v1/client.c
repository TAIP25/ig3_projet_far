#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <pthread.h>

#define MAXCHARS 255

void * messageReceive(void* pClient_socket) {

    //On récupère le descripteur de socket (on cast en long pour éviter un warning)
    int client_socket = (long) pClient_socket;

    char message[MAXCHARS] = {0};

    while(1) {

        //Le client attend la réponse du serveur
        int recvR = recv(client_socket, message, sizeof(message), 0) - 1;

        if (recvR == -1 || strcmp(message, "fin") == 0) {
            perror("Erreur lors de la réception du message ou fin de la conversation");
            exit(0);
        }

        printf("Interlocuteur : %s\n", message);

    }
}

void * messageSend(void * pClient_socket) {

    //On récupère le descripteur de socket (on cast en long pour éviter un warning)
    int client_socket = (long) pClient_socket;
    
    // N'importe quel client peut commencer la conversation
        printf("C'est le début de votre conversation\n");

    while(1) {

        char message[MAXCHARS] = {0};
                
        fgets(message, MAXCHARS, stdin);
        if(message[strlen(message) - 1] == '\n'){
            message[strlen(message) - 1] = '\0';
        }

        if(strlen(message) > MAXCHARS){
            printf("Erreur message trop grand\n");
        }
        else{
            //Envoie le message au serveur
            //int send(int dS, const void *m, size_t lg, int flags)
            //Renvoie le nombre d'octet envoyé si la connexion est réussi et -1 si elle échoue
            //dS = descripteur de socket
            //m = message
            //strlen(m) + 1 = taille du message
            //0 = protocole par défaut
            send(client_socket, message, strlen(message) + 1 , 0);
            if (strcmp(message, "fin") == 0) {
                perror("Erreur lors de la réception du message ou fin de la conversation");
                exit(0);
            }
            

        }
    }
}

int main(int argc, char *argv[]) {

    //VERIFICATION DU NOMBRE D'ARGUMENTS
    //argv[0] = client
    //argv[1] = adresse IP du serveur
    //argv[2] = port du serveur
    if (argc != 3) {
        printf("Erreur nombre d'argument\n");
        printf("Usage: %s <adresse IP> <port>\n", argv[0]);
        exit(0);
    }

    //CREATION DE LA SOCKET DU CLIENT

    //int socket(int domaine, int type, int protocole)
    //Renvoie le descripteur de socket si la création est réussi et -1 si elle échoue
    //PF_INET = Protocole IP
    //SOCK_STREAM = Protocole TCP
    //0 = Protocole par défaut
    int client_socket = socket(AF_INET, SOCK_STREAM, 0);

    //gestion d'erreur
    if (client_socket == -1) {
        perror("Erreur de création de socket client");
        exit(0);
    }
    printf("Socket Créé\n");


    //DEFINITION DE L'ADRESSE IP ET DU PORT D'ECOUTE DU SERVEUR

    //adresse du serveur
    struct sockaddr_in server_address;

    //Stock la famille d'adresse dans la structure aS
    //AF_INET = Protocole IP
    server_address.sin_family = AF_INET;

    //Converti l'adresse IP en format réseau
    //inet_pton(int af, const char *src, void *dst)
    //Renvoie 1 si la conversion est réussi et 0 si elle échoue
    //INADDR_ANY = adresse IP de la machine
    //argv[1] = adresse IP du serveur
    //server_adress.sin_addr = adresse du serveur
    inet_pton(AF_INET,argv[1],&(server_address.sin_addr));

    //Converti le port en format réseau
    //unsigned short htons(unsigned short hostshort)
    //atoi() is a function in the C standard library that converts a string of characters representing an integer value to an actual integer value.
    //Renvoie le port en format réseau en cas de succès et 0 en cas d'échec
    //argv[2] = port du serveur
    server_address.sin_port = htons(atoi(argv[2]));

    //Connecte la socket au serveur
    //connect(int dS, struct sockaddr *aS, socklen_t lgA)
    //Renvoie 0 si la connexion est réussi et -1 si elle échoue
    //dS = descripteur de socket
    //aS = adresse du serveur
    //lgA = taille de la structure aS
    int connection = connect(client_socket, (struct sockaddr*) &server_address, sizeof(server_address));
    if (connection == -1) {
        perror("Erreur lors de la connexion du client au serveur");
        exit(0);
    }


    //Cast en long pour éviter un warning
    pthread_t threadReceive;
    pthread_t threadSend;

    pthread_create(&threadReceive, NULL, messageReceive, (void *) (long) client_socket);
    pthread_create(&threadSend, NULL, messageSend, (void *) (long) client_socket);

    pthread_join(threadReceive, NULL);
    pthread_join(threadSend, NULL);

    // Fermeture de la socket du client
    close(client_socket);

    return 0;

}
