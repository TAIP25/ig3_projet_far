#include <pthread.h>
#include <semaphore.h>

#include <stdio.h>

#include <sys/socket.h>

#include "global.h"

//Liste des descripteurs de socket des clients
int dSCList[MAX_CLIENT];

//Création de thread pour les clients
//pthread_t est un type de donnée qui représente un thread
pthread_t threadC[MAX_CLIENT];

//Création de pseudo pour les clients
char pseudoList[MAX_CLIENT][MAX_PSEUDO];

//Création d'un mutex pour les variables partagées
//Il n'y a qu'un seul mutex car toutes les variables partagées étroitement liées
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

//Création d'un sémaphore indiquant le nombre de places restantes sur le serveur
sem_t semaphoreSlot;

//Création d'une liste de booléen pour savoir si un thread doit être terminé
int threadEnd[MAX_CLIENT];

//Création d'un sémaphore indiquant le nombre de threads à nettoyer
sem_t semaphoreMemory;