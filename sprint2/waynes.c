#include <stdio.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>

//Taille maximale de client connecté
#define MAX_CLIENT 10

//Taille maximale d'un message
//Attention, il faut aussi modifier au niveau du client
#define MAX_MSG 255

//Nombre maximum de token possible
#define MAX_TOKEN 4

//Liste des descripteurs de socket des clients
int dSCList[MAX_CLIENT];

//Création de thread pour les clients
//pthread_t est un type de donnée qui représente un thread
pthread_t threadC[MAX_CLIENT];

//Initialise la liste des descripteurs de socket des clients à -1
void initDSCList(){
    for(int i = 0; i < MAX_CLIENT; i++){
        dSCList[i] = -1;
    }
}

//msg = message reçu
//dSC = descripteur de socket du client qui a envoyé le message
//dSCList = liste des descripteurs de socket des clients
void sendToAll(char* msg, int dSC){
    for(int i = 0; i < MAX_CLIENT; i++){
        if(dSCList[i] != dSC && dSCList[i] != -1){
            //Envoie un message au client i
            //int send(int dSC, void *msg, int lg, int flags)
            //dSC = descripteur de socket du client
            //0 = flags
            send(dSCList[i], msg, MAX_MSG, 0);
        }
    }
}

void sendToSpecificClient(char* msg, int dSC){
    for(int i = 0; i < MAX_CLIENT; i++){
        if(dSCList[i] == dSC && dSCList[i] != -1){
            //Envoie un message au client i
            //int send(int dSC, void *msg, int lg, int flags)
            //dSC = descripteur de socket du client
            //0 = flags
            send(dSCList[i], msg, MAX_MSG, 0);
        }
    }
}

//Gère la connexion d'un client
//arg = descripteur de socket du client
void * clientReceive(void* arg){
    
    int i = (long) arg;

    char msg[MAX_MSG];

    while (1) {

        //Reçoit un message du client i
        //int recv(int dSC, void *msg, int lg, int flags)
        //dSC = descripteur de socket du client
        //msg = message reçu
        //sizeof(msg) = taille du message (ici 32)
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

        //sudo mp pour envoyer un message privé à un client
        //tableau pour storer les token
        // char **tokens = malloc(MAX_TOKEN * sizeof(char));
        // for(int i = 0; i < MAX_TOKEN; i++){
        //     tokens[i] = malloc(MAX_MSG * sizeof(char));
        // }

        printf("Message reçu : %s\n", msg);


        //Définir un protocole pour les commandes particulières envoyées en messages depuis le client (exemple @ sur discord)
        if (strncmp(msg, "sudo", 4) == 0){ //attention il faut mettre un espace après sudo
            //Si le message commence par sudo, c'est une commande
            //On peut donc faire des actions en fonction de la commande

            //tableau pour storer les token
            char **tokens = malloc(MAX_TOKEN * sizeof(char));
            //tokens = (char**) malloc(MAX_TOKEN * sizeof(char*)); //allocation mémoire pour le tableau de token
            //découpage du message en token: de base sudo mp va tout le temps devoir être traité
            char *command = strtok(msg, " "); //tok0 = sudo
            char *cmdType = strtok(NULL, " "); //tok1 = mp
            //on stock tok1 tok2 dans le tableau
            tokens[0] = command;
            tokens[1] = cmdType;
            
            //sudo quit pour quitter le serveur
            if(strcmp(msg, "sudo quit") == 0){
                break;
            }

            //sudo list pour afficher la liste des clients connectés
            else if(strcmp(msg, "sudo list") == 0){
                //Envoie la liste des clients connectés au client i
                //int send(int dSC, void *msg, int lg, int flags)
                send(dSCList[i], "Liste des clients connectés : ", 255, 0);
            }

            //sudo mp pour envoyer un message privé à un client
            else if(strcmp(tokens[0], "sudo") == 0 || strcmp(tokens[1], "mp") == 0){
                //Envoie un message privé du client i au client souhaité
                
                char *numClient = strtok(NULL, " "); //tok2 = numéro du client
                char *message = strtok(NULL, "\0"); //tok3 = message
                //message = strtok(NULL, "\0");  //ne prend pas compte les \0 pour la séparation en token
                tokens[2] = numClient;
                tokens[3] = message;

                int numDestinataire = atoi(tokens[2]);

                //a. première version avec utilisation du numéro de client ou un identifiant 
                //sudo mp <numéro du client> "message"
                sendToSpecificClient(tokens[3], dSCList[numDestinataire]);

            }

            //sudo help pour afficher la liste des commandes
            else if(strcmp(msg, "sudo help") == 0){
                //Envoie la liste des commandes au client i
                //int send(int dSC, void *msg, int lg, int flags)
                send(dSCList[i], "Liste des commandes : \n", 255, 0);
            }

            //sudo name pour choisir un pseudo
            else if(strcmp(msg, "sudo name") == 0){
                //Envoie un message au client i pour qu'il choisisse un pseudo
                //int send(int dSC, void *msg, int lg, int flags)
                send(dSCList[i], "Choisissez un pseudo : \n", 255, 0);
            }

            //sudo kick pour kick un autre client
            else if(strcmp(msg, "sudo kick") == 0){
                //Envoie un message au client i pour qu'il choisisse un client à kick
                //int send(int dSC, void *msg, int lg, int flags)
                send(dSCList[i], "A quel client voulez-vous kick ?\n", 255, 0);
            }

            //sudo rename pour se renommer
            else if(strcmp(msg, "sudo rename") == 0){
                //Envoie un message au client i pour qu'il choisisse un nouveau pseudo
                //int send(int dSC, void *msg, int lg, int flags)
                send(dSCList[i], "Choisissez un nouveau pseudo : \n", 255, 0);
            }

            free(tokens);

        }

        else {
                //Envoie le message reçu à tous les clients sauf celui qui l'a envoyé
                sendToAll(msg, dSCList[i]);
            }

        printf("Message envoyé\n");
        
    }


    printf("Fin de la connexion\n");
    //Ferme la connexion avec le client
    //int shutdown(int dSC, int mode)
    //Renvoie 0 si la fermeture est réussi et -1 si elle échoue
    //dSC = descripteur de socket du client
    //mode = mode de fermeture
    //0 = plus de réception
    //1 = plus d'envoi
    //2 = plus de réception et d'envoi
    shutdown(dSCList[i], 2); 

    //Supprime le descripteur de socket du client de la liste
    dSCList[i] = -1;

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

    printf("Début programme\n");
    
    //Initialise la liste des descripteurs de socket des clients à -1
    initDSCList();

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
    
    printf("Le socket est créé\n"); 

    //Liaison de la socket au serveur
    //int bind(int dS1, struct sockaddr *aS, socklen_t lgA)
    //Renvoie 0 si la liaison est réussi et -1 si elle échoue
    //dS1 = descripteur de socket
    //&aS = adresse du serveur
    //sizeof(aS) = taille de la structure aS
    bind(dS, (struct sockaddr*)&aS, sizeof(aS));

    printf("Le socket est nommé\n");

    //Mise en écoute de la socket
    //int listen(int dS, int nbC)
    //Renvoie 0 si la mise en écoute est réussi et -1 si elle échoue
    //dS = descripteur de socket
    //nbC = nombre de connexion en attente
    listen(dS, 2);

    printf("Mode écoute\n");

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
            printf("Le client %d est connecté\n", i);
            pthread_create(&threadC[i], NULL, clientReceive, (void *) (long) i);
            i++;
        }
    }

    printf("Fin de la conversation\n");

    //Ferme la connexion du serveur
    //int shutdown(int dS1, int mode)
    //Renvoie 0 si la fermeture est réussi et -1 si elle échoue
    //dS1 = descripteur de socket
    //2 = fermeture de la connexion
    shutdown(dS, 2);

    printf("Fin du programme\n");
}