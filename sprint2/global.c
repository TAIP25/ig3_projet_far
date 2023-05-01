#include <pthread.h>
#include "global.h"


//Liste des descripteurs de socket des clients
int dSCList[MAX_CLIENT];

//Création de thread pour les clients
//pthread_t est un type de donnée qui représente un thread
pthread_t threadC[MAX_CLIENT];

//Création de pseudo pour les clients
char pseudoList[MAX_CLIENT][MAX_PSEUDO];