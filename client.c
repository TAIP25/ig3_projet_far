#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>

int main(int argc, char *argv[]) {

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

    //Stocke la taille de la structure aS dans lgA pour la fonction connect()
    socklen_t lgA = sizeof(struct sockaddr_in);

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

    //DEMANDE DE MESSAGES

    // the while(1) loop is used to create an infinite loop that repeatedly prompts the user to enter a message,
    // sends the message to the server, and waits for a response from the server. The loop continues until the user enters the message "fin",
    // at which point the loop is terminated using a break statement.
    while (1) {
        // Le client envoie un message
        printf("Saisir un message : ");
        char message[1024] = {0};
        fgets(message, sizeof(message), stdin);
        message[strcspn(message, "\n")] = 0;
        write(client_socket, message, strlen(message));

        
        if (strcmp(message, "fin") == 0) { //compares the messgage to "fin". If the comparison == 0, end the conversation
            break;
        }

        // Le client attend la réponse du serveur
        char buffer[1024] = {0};
        int message_size = read(client_socket, buffer, sizeof(buffer));
        printf("Réponse du serveur : %s\n", buffer);
    }

    // Fermeture de la socket du client
    close(client_socket);

    return 0;

}   
