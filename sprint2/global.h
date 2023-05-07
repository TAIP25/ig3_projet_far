#include <netinet/in.h>
#include <semaphore.h>

// ifndef GLOBAL_H permet de vérifier si la constante GLOBAL_H est définie
// define GLOBAL_H permet de définir une constante GLOBAL_H
// Cette constante permet de vérifier si le fichier a déjà été inclus

#ifndef GLOBAL_H
#define GLOBAL_H

//Taille maximale de client connecté
#define MAX_CLIENT 2

//Taille maximale d'un message
//Attention, il faut aussi modifier au niveau du client
#define MAX_CHAR 1000

//Taille maximale d'un pseudo
#define MAX_PSEUDO 20

//Liste des descripteurs de socket des clients
extern int dSCList[MAX_CLIENT];

//Création de thread pour les clients
//pthread_t est un type de donnée qui représente un thread
extern pthread_t threadC[MAX_CLIENT];

//Création de pseudo pour les clients
extern char pseudoList[MAX_CLIENT][MAX_PSEUDO];

//Création d'un mutex pour les variables partagées
//Il n'y a qu'un seul mutex car toutes les variables partagées étroitement liées
extern pthread_mutex_t mutex;

//Création d'un sémaphore indiquant le nombre de places restantes sur le serveur
extern sem_t semaphoreSlot;

//Création d'une liste de booléen pour savoir si un thread doit être terminé
extern int threadEnd[MAX_CLIENT];

//Création d'un sémaphore indiquant le nombre de threads à nettoyer
extern sem_t semaphoreMemory;

#endif