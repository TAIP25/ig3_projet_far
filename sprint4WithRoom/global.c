#include <pthread.h>
#include <semaphore.h>

#include <stdio.h>

#include <sys/socket.h>

#include "global.h"

// Regroupe les informations d'un client dans une structure
// Cette structure est utilisée pour la liste des clients
// 
struct Client clientList[MAX_CLIENT];


// Regroupe les informations d'un salon dans une structure
// Cette structure est utilisée pour la liste des salons
struct Room roomList[MAX_ROOM];

// Création d'un mutex pour les variables partagées
// Il n'y a qu'un seul mutex car toutes les variables partagées étroitement liées
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

// Création d'un sémaphore indiquant le nombre de places restantes sur le serveur
sem_t semaphoreSlot;

// Création d'un sémaphore indiquant le nombre de threads à nettoyer
sem_t semaphoreMemory;

// Création d'une fonction permettant de savoir si un fichier existe
// Retourne 1 si le fichier existe, 0 sinon
int fileExist(char * filepath){
    FILE * file = fopen(filepath, "r");
    if(file == NULL){
        return 0;
    }
    else{
        fclose(file);
        return 1;
    }
}