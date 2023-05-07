#include <pthread.h>
#include <semaphore.h>

#include <stdio.h>

#include <sys/socket.h>

#include "global.h"

//Liste des descripteurs de socket des clients
int dSCList[MAX_CLIENT];

//Création d'un tableau statique d'addresse de socket
//aC = adresse du client
struct sockaddr_in aC[MAX_CLIENT];

//Stocke la taille de la structure aC dans lg
socklen_t lg[MAX_CLIENT];

//Création de thread pour les clients
//pthread_t est un type de donnée qui représente un thread
pthread_t threadC[MAX_CLIENT];

//Création de pseudo pour les clients
char pseudoList[MAX_CLIENT][MAX_PSEUDO];

//Création d'un mutex pour la liste des descripteurs de socket des clients et la liste des pseudos
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

//Création d'un sémaphore indiquant le nombre de places restantes sur le serveur
sem_t semaphore;

//Création d'une liste de booléen pour savoir si un thread doit être terminé
int threadEnd[MAX_CLIENT];