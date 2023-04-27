#ifndef COMMANDES_H
#define COMMANDES_H

// Vérifie si le client est connecté
// Renvoie 1 si le client est connecté, 0 sinon
int isConnected(int dSC);

// Recupère le dSC du client à partir de son id
// pre: isConnected(dSC) == 1
int getDSC(int id);

// Récupère l'id du client à partir de son dSC
// pre: isConnected(dSC) == 1
int getId(int dSC);

// Appelé quand le client envoie la commande "sudo all <msg>"
// Appelé aussi quand le client envoie "<msg>", car c'est la commande par défaut
void sendAll(char* msg, int dSC);

// Appelé quand le client envoie la commande "sudo mp <id> <msg>"
void sendMP(char* msg, int id);

// Appelé quand le client envoie la commande "sudo help"
void sendHelp(int dSC);

// Appelé quand le client envoie la commande "sudo quit"
void sendQuit(int dSC);

// Appelé quand le client envoie la commande "sudo list"
void sendList(int dSC);

// Appelé quand le client envoie la commande "sudo kick <id>"
void kickClient(int id);

#endif