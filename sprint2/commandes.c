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
// post: Attention, si le client n'est pas connecté => return -1
int getDSCByPseudo(char* pseudo){
    for(int i = 0; i < MAX_CLIENT; i++){
        if(strcmp(pseudoList[i], pseudo) == 0){
            return i;
        }
    }
    return -1;
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
    
    char infoMsg[MAX_CHAR] = "[BROADCAST] ";
    strcat(infoMsg, msg);

    for(int i = 0; i < MAX_CLIENT; i++){
        if(dSCList[i] != dSC && dSCList[i] != -1){
            
            // Envoie un message au client i
            // int send(int dSC, void *msg, int lg, int flags)
            // Renvoie le nombre d'octet envoyé
            // dSC = descripteur de socket du client
            // &recvR = message envoyé
            // sizeof(int) = taille du message (ici 4)
            // 0 = flags
            if(send(dSCList[i], infoMsg, MAX_CHAR, 0) == -1){
                perror("Erreur lors de l'envoie du message");
                exit(0);
            }
        }
    }
}

// Appelé quand le client envoie la commande "sudo mp <id> <msg>"
// pre: isConnected(dSC) == 1
// post: Attention, si l'un des clients n'est pas connecté, une erreur est throw
// post: Attention, si l'envoie du message échoue, une erreur est throw
void sendMP(char* msg, int idS, int idR){
    if(isConnected(getDSC(idS)) == 0 || isConnected(getDSC(idR)) == 0){
        perror("Erreur un des clients n'est pas connecté");
        exit(0);
    }

    // Envoie un message au client idR
    char infoMsg[MAX_CHAR] = "[MP] ";
    strcat(infoMsg, getPseudoByDSC(getDSC(idS)));
    strcat(infoMsg, ": ");
    strcat(infoMsg, msg);

    if(send(getDSC(idR), infoMsg, MAX_CHAR, 0) == -1){
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
    
    // Ouverture du fichier commandes.txt
    FILE *f = fopen("commandes.txt", "r");
    if(f == NULL){
        perror("Erreur lors de l'ouverture du fichier");
        exit(0);
    }
    char line[MAX_CHAR];
    while(fgets(line, sizeof(line), f)){
        line[strlen(line)-1] = '\0';
        if(send(dSC, line, sizeof(line), 0) == -1){
            perror("Erreur lors de l'envoie du message");
            exit(0);
        }
    }
    fclose(f);
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
    char quit[MAX_CHAR] = "[INFO] Vous avez été déconnecté du serveur";
    if(send(dSC, quit, MAX_CHAR, 0) == -1){
        perror("Erreur lors de l'envoie du message");
        exit(0);
    }

    // Ferme la connexion avec le client
    // int shutdown(int dSC, int mode)
    // Renvoie 0 si la fermeture est réussi et -1 si elle échoue
    // dSC = descripteur de socket du client
    // mode = mode de fermeture
    // 0 = plus de réception
    // 1 = plus d'envoi
    // 2 = plus de réception et d'envoi
    if(shutdown(dSC, 2) == -1){
        perror("Erreur lors de la fermeture de la connexion avec le client");
        exit(0);
    }

    pthread_mutex_lock(&mutex);
    dSCList[id] = -1;
    pthread_mutex_unlock(&mutex);

    printf("[INFO] Le client %s s'est déconnecté\n", pseudoList[id]);

    pthread_mutex_lock(&mutex);
    sprintf(pseudoList[id], "Client%d", id);
    pthread_mutex_unlock(&mutex);

    // Exit du thread
    // void pthread_exit(void *retval);
    // retval = valeur de retour
    pthread_exit(&threadC[id]);
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
    char list[MAX_CHAR*MAX_CLIENT/10] = "[INFO] Liste des clients connectés :\n";
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
            strcat(list, "[INFO] ");
            strcat(list, id);
            strcat(list, ": ");
            strcat(list, pseudo);
            strcat(list, "\n");
        }
    }
    // Supprime le dernier \n
    list[strlen(list)-1] = '\0';

    if(send(dSC, list, MAX_CHAR, 0) == -1){
        perror("Erreur lors de l'envoie du message");
        exit(0);
    }
}

// Appelé quand le client envoie la commande "sudo kick <id>"
// pre: isConnected(dSC) == 1
// post: Attention, si l'un des clients n'est pas connecté, une erreur est throw
// post: Attention, si l'envoie du message échoue, une erreur est throw
// post: Attention, si la fermeture de la connexion échoue, une erreur est throw
void sendKick(int idS, int idR){
    if(isConnected(getDSC(idS)) == 0 || isConnected(getDSC(idR)) == 0){
        perror("Erreur le client n'est pas connecté");
        exit(0);
    }
    
    // Envoie un message de prévention au client
    char kick[MAX_CHAR] = "[INFO] Vous avez été kick du serveur par ";
    strcat(kick, pseudoList[idS]);

    if(send(getDSC(idR), kick, MAX_CHAR, 0) == -1){
        perror("Erreur lors de l'envoie du message");
        exit(0);
    }

    // Ferme la connexion avec le client
    // int shutdown(int dSC, int mode)
    // Renvoie 0 si la fermeture est réussi et -1 si elle échoue
    // dSC = descripteur de socket du client
    // mode = mode de fermeture
    // 0 = plus de réception
    // 1 = plus d'envoi
    // 2 = plus de réception et d'envoi
    if(shutdown(getDSC(idR), 2) == -1){
        perror("Erreur lors de la fermeture de la connexion avec le client");
        exit(0);
    }

    pthread_mutex_lock(&mutex);
    dSCList[idR] = -1;
    pthread_mutex_unlock(&mutex);

    printf("[INFO] Le client %s s'est fait kick\n", pseudoList[idR]);

    pthread_mutex_lock(&mutex);
    sprintf(pseudoList[idR], "Client%d", idR);
    pthread_mutex_unlock(&mutex);
    
    // Exit du thread
    // void pthread_exit(void *retval);
    // retval = valeur de retour
    pthread_exit(&threadC[idR]);
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

        printf("[INFO] Le client %s a changé son pseudo en %s\n", pseudoList[getID(dSC)], pseudo);
        
        //pthread_mutex_lock(&mutex);
        strcpy(pseudoList[getID(dSC)], pseudo);
        //pthread_mutex_unlock(&mutex);

        char rename[MAX_CHAR] = "[INFO] Votre pseudo a été changé en ";
        strcat(rename, pseudo);

        if(send(dSC, rename, MAX_CHAR, 0) == -1){
            perror("Erreur lors de l'envoie du message");
            exit(0);
        }
    }
    else{
        char rename[MAX_CHAR] = "[INFO] Votre pseudo n'a pas pu être changé car il est déjà pris ou contient des caractères interdits ou est trop long réessayez avec sudo rename <pseudo>";
        if(send(dSC, rename, MAX_CHAR, 0) == -1){
            perror("Erreur lors de l'envoie du message");
            exit(0);
        }
    }
}