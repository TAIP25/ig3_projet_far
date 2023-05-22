#include <pthread.h>
#include <semaphore.h>

#include <stdio.h>

#include <sys/socket.h>

#include "global.h"

struct Client clientList[MAX_CLIENT];

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