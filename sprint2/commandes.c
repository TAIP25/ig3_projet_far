#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <pthread.h>
#include <sys/socket.h>
#include <arpa/inet.h>

#include "global.h"

// Vérifie si le client est connecté
// pre:
// post: isConnected(dSC) == 1 || isConnected(dSC) == 0
int isConnected(int dSC){
    for(int i = 0; i < MAX_CLIENT; i++){
        if(dSCList[i] == dSC){
            return 1;
        }
    }
    return 0;
}

// Recupère la dSC du client à partir de son id
// pre: isConnected(dSC) == 1
// post: getDSC(getID(dSC)) == dSC
// post: getDSC(id) == dSC
// post: Attention, si le client n'est pas connecté, une erreur est throw
int getDSC(int id){
    // Overkill mais mieux si il y a un scale up
    if(isConnected(dSCList[id])){
        return dSCList[id];
    }
    else{
        perror("Erreur lors de la récupération du descripteur de socket du client");
        exit(0);
    }
    
}

// Recupère l'id du client à partir de sa dSC
// pre: isConnected(dSC) == 1
// post: getID(getDSC(id)) == id
// post: getID(dSC) == id, 0 <= id < MAX_CLIENT
// post: Attention, si le client n'est pas connecté, une erreur est throw
int getID(int dSC){
    for(int i = 0; i < MAX_CLIENT; i++){
        if(dSCList[i] == dSC){
            return i;
        }
    }
    perror("Erreur lors de la récupération de l'id du client");
    exit(0);
}

// Recupère le pseudo du client à partir de sa dSC
// pre: isConnected(dSC) == 1
// post: getPseudo(dSC) == pseudo
// post: Attention, si le client n'est pas connecté, une erreur est throw
char* getPseudoByDSC(int dSC){
    if(isConnected(dSC) == 0){
        perror("Erreur le client n'est pas connecté");
        exit(0);
    }
    return pseudoList[getID(dSC)];
}

// Recupère le dSC du client à partir de son pseudo
// pre: isConnected(dSC) == 1
// post: getDSC(pseudo) == dSC
// post: Attention, si le client n'est pas connecté, une erreur est throw
int getDSCByPseudo(char* pseudo){
    for(int i = 0; i < MAX_CLIENT; i++){
        if(strcmp(pseudoList[i], pseudo) == 0){
            return i;
        }
    }
    perror("Erreur lors de la récupération du descripteur de socket du client");
    exit(0);
}

// Appelé quand le client envoie la commande "sudo all <msg>"
// Appelé aussi quand le client envoie "<msg>", car c'est la commande par défaut
// pre: isConnected(dSC) == 1
// post: Attention, si le client n'est pas connecté, une erreur est throw
// post: Attention, si l'envoie du message échoue, une erreur est throw
void sendAll(char* msg, int dSC){
    if(isConnected(dSC) == 0){
        perror("Erreur le client n'est pas connecté");
        exit(0);
    }
    for(int i = 0; i < MAX_CLIENT; i++){
        if(dSCList[i] != dSC && dSCList[i] != -1){
            
            //Envoie un message au client i
            //int send(int dSC, void *msg, int lg, int flags)
            //Renvoie le nombre d'octet envoyé
            //dSC = descripteur de socket du client
            //&recvR = message envoyé
            //sizeof(int) = taille du message (ici 4)
            //0 = flags
            if(send(dSCList[i], msg, MAX_CHAR, 0) == -1){
                perror("Erreur lors de l'envoie du message");
                exit(0);
            }
        }
    }
}

// Appelé quand le client envoie la commande "sudo mp <id> <msg>"
// pre: isConnected(dSC) == 1
// post: Attention, si le client n'est pas connecté, une erreur est throw
// post: Attention, si l'envoie du message échoue, une erreur est throw
void sendMP(char* msg, int id){
    if(isConnected(getDSC(id)) == 0){
        perror("Erreur le client n'est pas connecté");
        exit(0);
    }
    if(send(getDSC(id), msg, MAX_CHAR, 0) == -1){
        perror("Erreur lors de l'envoie du message");
        exit(0);
    }
}

// Appelé quand le client envoie la commande "sudo help"
// pre: isConnected(dSC) == 1
// post: Attention, si le client n'est pas connecté, une erreur est throw
// post: Attention, si l'envoie du message échoue, une erreur est throw
void sendHelp(int dSC){
    if(isConnected(dSC) == 0){
        perror("Erreur le client n'est pas connecté");
        exit(0);
    }
    char help[MAX_CHAR] = "Liste des commandes disponibles :\n"
                 "sudo help : affiche la liste des commandes disponibles\n"
                 "sudo all <msg> : envoie un message à tous les clients connectés\n"
                 "sudo mp <id> <msg> : envoie un message privé au client d'id <id>\n"
                 "sudo quit : déconnecte le client du serveur\n"
                 "sudo list : affiche la liste des clients connectés\n"
                 "sudo kick <id> : déconnecte le client d'id <id> du serveur\n";

    if(send(dSC, help, MAX_CHAR, 0) == -1){
        perror("Erreur lors de l'envoie du message");
        exit(0);
    }
}

// Appelé quand le client envoie la commande "sudo quit"
// pre: isConnected(dSC) == 1
// post: Attention, si le client n'est pas connecté, une erreur est throw
// post: Attention, si l'envoie du message échoue, une erreur est throw
// post: Attention, si la fermeture de la connexion échoue, une erreur est throw
void sendQuit(int dSC){
    
    int id = getID(dSC);

    if(isConnected(dSC) == 0){
        perror("Erreur le client n'est pas connecté");
        exit(0);
    }

    // Envoie un message de prévention au client
    char quit[MAX_CHAR] = "Vous avez été déconnecté du serveur\n";
    if(send(dSC, quit, MAX_CHAR, 0) == -1){
        perror("Erreur lors de l'envoie du message");
        exit(0);
    }

    //Ferme la connexion avec le client
    //int shutdown(int dSC, int mode)
    //Renvoie 0 si la fermeture est réussi et -1 si elle échoue
    //dSC = descripteur de socket du client
    //mode = mode de fermeture
    //0 = plus de réception
    //1 = plus d'envoi
    //2 = plus de réception et d'envoi
    if(shutdown(dSC, 2) == -1){
        perror("Erreur lors de la fermeture de la connexion avec le client");
        exit(0);
    }
    
    dSCList[id] = -1;

    printf("Le client %s s'est déconnecté\n", pseudoList[id]);

    sprintf(pseudoList[id], "Client%d", id);
    
    //Exit du thread
    //void pthread_exit(void *retval);
    //retval = valeur de retour
    pthread_exit(&threadC[getID(dSC)]);
}

// Appelé quand le client envoie la commande "sudo list"
// pre: isConnected(dSC) == 1
// post: Attention, si le client n'est pas connecté, une erreur est throw
// post: Attention, si l'envoie du message échoue, une erreur est throw
void sendList(int dSC){
    if(isConnected(dSC) == 0){
        perror("Erreur lors de l'envoie du message");
        exit(0);
    }
    char list[MAX_CHAR*MAX_CLIENT/10] = "Liste des clients connectés :\n";
    for(int i = 0; i < MAX_CLIENT; i++){
        if(dSCList[i] != -1){
            
            // Permet de si MAX_CHAR change de valeur il n'y ai pas de problème

            // Converti l'id en char*
            // int snprintf(char *str, size_t size, const char *format, ...)
            // Renvoie le nombre d'octet écrit dans str
            // str = chaine de caractère dans laquelle écrire
            // size = taille de la chaine de caractère
            // format = format de la chaine de caractère
            // ... = variable à écrire dans la chaine de caractère
            int nId = snprintf(NULL, 0, "%d", i);
            char id[nId+1];
            int nPseudo = snprintf(NULL, 0, "%s", pseudoList[i]);
            char pseudo[nPseudo+1];
            snprintf(id, nId+1, "%d", i);
            snprintf(pseudo, nPseudo+1, "%s", pseudoList[i]);

            // Ajoute l'id à la liste
            // char *strcat(char *dest, const char *src)
            // Renvoie dest
            // dest = chaine de caractère dans laquelle ajouter src
            // src = chaine de caractère à ajouter à dest
            strcat(list, id);
            strcat(list, " : ");
            strcat(list, pseudo);
            strcat(list, "\n");
        }
    }
    if(send(dSC, list, MAX_CHAR, 0) == -1){
        perror("Erreur lors de l'envoie du message");
        exit(0);
    }
}

// Appelé quand le client envoie la commande "sudo kick <id>"
// pre: isConnected(dSC) == 1
// post: Attention, si le client n'est pas connecté, une erreur est throw
// post: Attention, si l'envoie du message échoue, une erreur est throw
// post: Attention, si la fermeture de la connexion échoue, une erreur est throw
void sendKick(int id){
    if(isConnected(getDSC(id)) == 0){
        perror("Erreur le client n'est pas connecté");
        exit(0);
    }
    // Envoie un message de prévention au client
    char kick[MAX_CHAR] = "Vous avez été kick du serveur par un client\n";
    if(send(getDSC(id), kick, MAX_CHAR, 0) == -1){
        perror("Erreur lors de l'envoie du message");
        exit(0);
    }

    //Ferme la connexion avec le client
    //int shutdown(int dSC, int mode)
    //Renvoie 0 si la fermeture est réussi et -1 si elle échoue
    //dSC = descripteur de socket du client
    //mode = mode de fermeture
    //0 = plus de réception
    //1 = plus d'envoi
    //2 = plus de réception et d'envoi
    if(shutdown(getDSC(id), 2) == -1){
        perror("Erreur lors de la fermeture de la connexion avec le client");
        exit(0);
    }

    dSCList[id] = -1;

    printf("Le client %s s'est fait kick\n", pseudoList[id]);

    sprintf(pseudoList[id], "Client%d", id);
    
    //Exit du thread
    //void pthread_exit(void *retval);
    //retval = valeur de retour
    pthread_exit(&threadC[id]);
}

// Vérifie si le pseudo est correct
// pre: pseudo != NULL
// post: Renvoie 1 si le pseudo est correct, 0 sinon
int properPseudo(char* pseudo){
    if(pseudo == NULL){
        perror("Erreur le pseudo est NULL");
        exit(0);
    }
    else if(strlen(pseudo) > MAX_PSEUDO){
        return 0;
    }
    for(int i = 0; i < MAX_CLIENT; i++){
        if(strcmp(pseudo, pseudoList[i]) == 0){
            return 0;
        }
    }
    for(int i = 0; i < strlen(pseudo); i++){
        if(pseudo[i] == ' ' || pseudo[i] == '\n' || pseudo[i] == '\t' || pseudo[i] == '\r'){
            return 0;
        }
    }
    return 1;
}

// Appelé quand le client envoie la commande "sudo rename <pseudo>"
// pre: isConnected(dSC) == 1
// post: Attention, si le client n'est pas connecté, une erreur est throw
// post: Attention, si l'envoie du message échoue, une erreur est throw
void sendRename(char* pseudo, int dSC){
    if(isConnected(dSC) == 0){
        perror("Erreur le client n'est pas connecté");
        exit(0);
    }
    
    // Avant de changer le pseudo, on vérifie qu'il n'est pas déjà pris, si c'est le cas on envoie un message pour prévenir le client que le pseudo est déjà pris
    // On vérifie aussi que le pseudo ne contient pas de caractère interdit d'espace, de retour à la ligne etc ...
    if(properPseudo(pseudo) == 1){

        printf("Le client %s a changé son pseudo en %s\n", pseudoList[getID(dSC)], pseudo);
        
        strcpy(pseudoList[getID(dSC)], pseudo);

        char rename[MAX_CHAR] = "Votre pseudo a été changé en ";
        strcat(rename, pseudo);
        strcat(rename, "\n");

        if(send(dSC, rename, MAX_CHAR, 0) == -1){
            perror("Erreur lors de l'envoie du message");
            exit(0);
        }
    }
    else{
        char rename[MAX_CHAR] = "Votre pseudo n'a pas pu être changé car il est déjà pris ou contient des caractères interdits ou est trop long\n";
        if(send(dSC, rename, MAX_CHAR, 0) == -1){
            perror("Erreur lors de l'envoie du message");
            exit(0);
        }
    }
}