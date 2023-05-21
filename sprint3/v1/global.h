#include <netinet/in.h>
#include <semaphore.h>

// ifndef GLOBAL_H permet de vérifier si la constante GLOBAL_H est définie
// define GLOBAL_H permet de définir une constante GLOBAL_H
// Cette constante permet de vérifier si le fichier a déjà été inclus

#ifndef GLOBAL_H
#define GLOBAL_H

// Taille maximale de client connecté
#define MAX_CLIENT 10

// Taille maximale d'un message
// Attention, il faut aussi modifier au niveau du client
#define MAX_CHAR 1000

// Taille maximale d'un pseudo
#define MAX_PSEUDO 20

// Taille maximale d'un nom de fichier
#define MAX_FILENAME 50

// Chemin du dossier de transfert du client
#define CLIENT_TRANSFER_FOLDER "transferClient/"

// Chemin du dossier de transfert du serveur
#define SERVER_TRANSFER_FOLDER "transferServeur/"

// Regroupe les informations d'un client dans une structure
// Cette structure est utilisée pour la liste des clients
struct Client {
    struct Connection {
        // Descripteur de socket du client
        int dSC;
        // Adresse du client
        struct sockaddr_in clientAddr;
        // Taille de l'adresse du client
        socklen_t clientAddrLen;
    } connection;
    // Pseudo du client
    char pseudo[MAX_PSEUDO];
    // Thread du client
    pthread_t thread;
    // Booléen indiquant si le thread doit être nettoyé
    int threadEnd;
} extern clientList[MAX_CLIENT];

// Création d'un mutex pour les variables partagées
// Il n'y a qu'un seul mutex car toutes les variables partagées étroitement liées
extern pthread_mutex_t mutex;

// Création d'un sémaphore indiquant le nombre de places restantes sur le serveur
extern sem_t semaphoreSlot;

// Création d'un sémaphore indiquant le nombre de threads à nettoyer
extern sem_t semaphoreMemory;

#endif