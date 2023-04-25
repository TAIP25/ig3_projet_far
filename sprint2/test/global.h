// ifndef GLOBAL_H permet de vérifier si la constante GLOBAL_H est définie
// define GLOBAL_H permet de définir une constante GLOBAL_H
// Cette constante permet de vérifier si le fichier a déjà été inclus

#ifndef GLOBAL_H
#define GLOBAL_H

//Taille maximale de client connecté
#define MAX_CLIENT 10

//Taille maximale d'un message
//Attention, il faut aussi modifier au niveau du client
#define MAX_CHAR 255

//Liste des descripteurs de socket des clients
extern int dSCList[MAX_CLIENT];


//Création de thread pour les clients
//pthread_t est un type de donnée qui représente un thread
extern pthread_t threadC[MAX_CLIENT];


#endif