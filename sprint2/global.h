// ifndef GLOBAL_H permet de vérifier si la constante GLOBAL_H est définie
// define GLOBAL_H permet de définir une constante GLOBAL_H
// Cette constante permet de vérifier si le fichier a déjà été inclus

#ifndef GLOBAL_H
#define GLOBAL_H

// Besoin de ceci au prochain sprint
//#include <pthread.h>

//Taille maximale de client connecté
#define MAX_CLIENT 10

//Taille maximale d'un message
//Attention, il faut aussi modifier au niveau du client
#define MAX_MSG 255

//Liste des descripteurs de socket des clients
extern int dSCList[MAX_CLIENT];

// Besoin de ceci au prochain sprint
//extern pthread_mutex_t mutex_compteur;

#endif