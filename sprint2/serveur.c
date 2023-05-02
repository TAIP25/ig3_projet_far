#include <stdio.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include "global.h"
#include "commandes.h"

//Initialise la liste des descripteurs de socket des clients à -1
void initDSCList(){
    for(int i = 0; i < MAX_CLIENT; i++){
        dSCList[i] = -1;
    }
}

//Initialise les pseudos des clients à l'id du client
void initPseudoList(){
    for(int i = 0; i < MAX_CLIENT; i++){
        sprintf(pseudoList[i], "Client%d", i);
    }
}

//Gère la connexion d'un client
//arg = descripteur de socket du client
void * clientReceive(void* arg){
    
    int i = (long) arg;

    char msg[MAX_CHAR];

    while (1) {

        //Reçoit un message du client i
        //int recv(int dSC, void *msg, int lg, int flags)
        //Renvoie le nombre d'octet reçu
        //dSC = descripteur de socket du client
        //msg = message reçu
        //sizeof(msg) = taille du message
        //0 = flags
        int recvC = recv(dSCList[i], msg, sizeof(msg), 0);

        //Vérifie si la connexion est interrompu ou si une erreur est survenue
        if(recvC == 0 ){
            break;
        }
        else if(recvC == -1){
            perror("Erreur lors de la réception du message");
            exit(0);
        }

        //printf("Message reçu : %s\n", msg);

        //Permet de comparer les chaines de caractères
        //int strncmp(const char *s1, const char *s2, size_t n)
        //Renvoie 0 si les chaines sont identiques
        //s1 = première chaine de caractère
        //s2 = deuxième chaine de caractère
        //n = nombre de caractère à comparer
        if(strncmp(msg, "sudo", 4) == 0){

            //Définis un protocol pour les commandes
            //Crée des tokens à partir du message reçu
            //Premier token = sudo
            //Deuxième token = commande
            //Troisième token = argument (optionnel)
            //Quatrième token = message (optionnel)

            //Permet de découper une chaine de caractère en plusieurs tokens
            //char* strtok(char *s, const char *delim)
            //Renvoie un pointeur sur le premier token trouvé
            //s = chaine de caractère à découper, NULL pour continuer à découper la chaine précédente
            //delim = caractère de séparation
            
            //Inutile de stocker le pointeur retourné, car on sait que le premier token est "sudo"
            strtok(msg, " ");

            char* commande = strtok(NULL, " ");
            
            char* arg;
            if(strncmp(commande, "mp", 2) == 0 || strncmp(commande, "kick", 4) == 0 || strncmp(commande, "rename", 6) == 0){
                arg = strtok(NULL, " ");
            }
            
            char* message;
            if(strncmp(commande, "all", 3) == 0 || strncmp(commande, "mp", 2) == 0){
                message = strtok(NULL, "\0");
            }

            /*
            //Vérifie les commandes avec un printf
            printf("commande = %s\n", commande);
            printf("arg = %s\n", arg);
            printf("message = %s\n", message);
            */

            //Vérifie si la commande est "sudo help"
            if(strncmp(commande, "all", 3) == 0){
                //Envoie le message à tous les clients
                sendAll(message, getDSC(i));
            }
            //Vérifie si la commande est "sudo mp <id> <msg>"
            else if(strncmp(commande, "mp", 2) == 0){
                //Envoie le message au client <id>
                sendMP(message, getDSCByPseudo(arg));
            }
            //Vérifie si la commande est "sudo help"
            else if(strncmp(commande, "help", 4) == 0){
                //Envoie la liste des commandes au client
                sendHelp(getDSC(i));
            }
            //Vérifie si la commande est "sudo quit"
            else if(strncmp(commande, "quit", 4) == 0){
                //Envoie un message de déconnexion au client
                sendKick(i);
                sendQuit(getDSC(i));
            }
            //Vérifie si la commande est "sudo list"
            else if(strncmp(commande, "list", 4) == 0){
                //Envoie la liste des clients au client
                sendList(getDSC(i));
            }
            //Vérifie si la commande est "sudo kick <id>"
            else if(strncmp(commande, "kick", 4) == 0){
                //Envoie un message de déconnexion au client <id>
                sendKick(getDSCByPseudo(arg));
            }
            //Vérifie si la commande est "sudo rename <pseudo>"
            else if(strncmp(commande, "rename", 6) == 0){
                //Change le pseudo du client
                sendRename(arg, getDSC(i));
            }
            else{

                //Avertis le client que la commande n'existe pas
                char* errorMsg = "Commande inconnue, tapez \"sudo help\" pour afficher la liste des commandes";
                send(getDSC(i), errorMsg, strlen(errorMsg) + 1, 0); 
            }
        }
        else{
            //Envoie le message reçu à tous les clients (commande par défaut)
            sendAll(msg, getDSC(i));
        }
    }

    //Exit du thread
    //void pthread_exit(void *retval);
    //retval = valeur de retour
    pthread_exit(&threadC[i]);
}

int main(int argc, char *argv[]) {

    //Vérification du nombre d'argument
    //argv[0] = port du serveur
    if(argc != 2){
        printf("Erreur nombre d'argument\n");
        exit(0);
    }
    
    //Initialise la liste des descripteurs de socket des clients à -1
    initDSCList();

    //Initialise les pseudos des clients à l'id du client
    initPseudoList();

    //Création de la structure aS
    //aS = adresse du serveur
    struct sockaddr_in aS;

    //Stocke la famille d'adresse dans la structure aS
    //AF_INET = Protocole IP
    aS.sin_family = AF_INET;

    //Converti l'adresse IP en format réseau
    //INADDR_ANY = adresse IP de la machine
    aS.sin_addr.s_addr = INADDR_ANY ;

    //Converti le port en format réseau
    //unsigned short htons(unsigned short hostshort)
    //Renvoie le port en format réseau en cas de succès et 0 en cas d'échec
    //argv[1] = port du serveur
    aS.sin_port = htons(atoi(argv[1])) ;

    //Création du socket pour les clients
    //int socket(int domaine, int type, int protocole)
    //Renvoie le descripteur de socket si la création est réussi et -1 si elle échoue
    //dS = descripteur de socket
    //PF_INET = Protocole IP
    //SOCK_STREAM = Protocole TCP
    //0 = Protocole par défaut
    int dS = socket(PF_INET, SOCK_STREAM, 0);
    
    //Le socket est créé 

    //Liaison de la socket au serveur
    //int bind(int dS1, struct sockaddr *aS, socklen_t lgA)
    //Renvoie 0 si la liaison est réussi et -1 si elle échoue
    //dS1 = descripteur de socket
    //&aS = adresse du serveur
    //sizeof(aS) = taille de la structure aS
    bind(dS, (struct sockaddr*)&aS, sizeof(aS));

    //Le socket est nommé

    //Mise en écoute de la socket
    //int listen(int dS, int nbC)
    //Renvoie 0 si la mise en écoute est réussi et -1 si elle échoue
    //dS = descripteur de socket
    //nbC = nombre de connexion en attente
    listen(dS, 2);

    //Création d'un tableau statique de 10 addresse de socket
    //aC = adresse du client
    struct sockaddr_in aC[MAX_CLIENT];
    

    //Stocke la taille de la structure aC dans lg
    socklen_t lg[MAX_CLIENT];

    for (int i = 0; i < MAX_CLIENT; i++) {
        lg[i] = sizeof(aC[i]);
    }

    //Gestion des clients
    //Dès qu'un client se connecte, un thread est créé pour gérer la connexion

    printf("Serveur lancé\n");
    
    int i = 0;
    while(1) {
        if(i == MAX_CLIENT){
            i = 0;
        }
        if(dSCList[i] == -1){
            //Accepte la connexion du client
            //int accept(int dS, struct sockaddr *aC, socklen_t *lg)
            //Renvoie le descripteur de socket du client
            //dS = descripteur de socket
            //&aC = adresse du client
            //&lg = l'adresse de la taille de la structure aC
            dSCList[i] = accept(dS, (struct sockaddr*) &aC[i],&lg[i]);
            printf("Le client %s est connecté\n", pseudoList[i]);
            pthread_create(&threadC[i], NULL, clientReceive, (void *) (long) i);
            i++;
        }
    }

    //Ferme la connexion du serveur
    //int shutdown(int dS1, int mode)
    //Renvoie 0 si la fermeture est réussi et -1 si elle échoue
    //dS1 = descripteur de socket
    //2 = fermeture de la connexion
    shutdown(dS, 2);

    printf("Le serveur est fermé\n");
}