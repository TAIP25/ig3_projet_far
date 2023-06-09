#include <stdio.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>
#include <signal.h>

#include "global.h"
#include "commandes.h"

// Création d'un tableau statique d'addresse de socket
// aC = adresse du client
struct sockaddr_in aC[MAX_CLIENT];

// Stocke la taille de la structure aC dans lg
socklen_t lg[MAX_CLIENT];

// Descripteur de socket du serveur
int dS; 

// Initialise la liste des descripteurs de socket des clients à -1
void initDSCList(){
    for(int i = 0; i < MAX_CLIENT; i++){
        dSCList[i] = -1;
    }
}

// Initialise les pseudos des clients à l'id du client
void initPseudoList(){
    for(int i = 0; i < MAX_CLIENT; i++){
        sprintf(pseudoList[i], "Client%d", i);
    }
}

// Initialise la liste de structure d'adresse de socket
void initAC(){
    for(int i = 0; i < MAX_CLIENT; i++){
        lg[i] = sizeof(aC[i]);
    }
}

// Initialise la liste de booléen à 0
void initThreadEnd(){
    for(int i = 0; i < MAX_CLIENT; i++){
        threadEnd[i] = 0;
    }
}


// Gère la connexion d'un client
// arg = l'indice du descripteur de socket du client
void * clientReceive(void* arg){
    
    int i = (long) arg;

    char msg[MAX_CHAR];

    char msgWait[MAX_CHAR] = "\033[36m[INFO]\033[0m Vous êtes sorti de la file d'attente, bienvenue sur le serveur";

    if(send(getDSC(i), msgWait, strlen(msgWait) + 1, 0) == -1){
        perror("Erreur lors de l'envoie du message");
        exit(0);
    }

    printf("\033[36m[INFO]\033[0m Un client s'est connecté\n");

    char msgPseudo[MAX_CHAR] = "\033[33m[NEED]\033[0m Veuillez entrer votre pseudo: ";
    msgPseudo[strlen(msgPseudo) - 1] = '\0';
    
    if(send(getDSC(i), msgPseudo, strlen(msgPseudo) + 1, 0) == -1){
        perror("Erreur lors de l'envoie du message");
        exit(0);
    }

    // Reçoit le pseudo du client
    int recvPseudo = recv(getDSC(i), msg, sizeof(msg), 0);
    if(recvPseudo == -1){
        perror("Erreur lors de la réception du message");
        exit(0);
    }
    else if(recvPseudo == 0){
        printf("\033[36m[INFO]\033[0m Un client s'est déconnecté sans informer le serveur\n");
        sendQuit(getDSC(i));
    }
    
    //Dans le cas où le client quite lors de la demande de pseudo
    if(strcmp(msg, "sudo quit") == 0){
        sendQuit(getDSC(i));
    }
    else{
        sendRename(msg, getDSC(i));
    }

    // Envoie un message de bienvenue au client
    char msgWelcome[MAX_CHAR] = "\033[36m[INFO]\033[0m C'est le début de votre conversation. Pour voir la liste des commandes faites \"sudo help\"";
    
    if(send(getDSC(i), msgWelcome, strlen(msgWelcome) + 1, 0) == -1){
        perror("Erreur lors de l'envoie du message");
        exit(0);
    }

    while (!threadEnd[i]) {

        // Reçoit un message du client i
        // int recv(int dSC, void *msg, int lg, int flags)
        // Renvoie le nombre d'octet reçu
        // dSC = descripteur de socket du client
        // msg = message reçu
        // sizeof(msg) = taille du message
        // 0 = flags
        int recvC = recv(dSCList[i], msg, sizeof(msg), 0);

        // Vérifie si la connexion est interrompu ou si une erreur est survenue
        if(recvC == 0 ){
            break;
        }
        else if(recvC == -1){
            perror("Erreur lors de la réception du message B");
            exit(0);
        }

        // printf("\033[36m[INFO]\033[0m Message reçu : %s\n", msg);

        // Permet de comparer les chaines de caractères
        // int strncmp(const char *s1, const char *s2, size_t n)
        // Renvoie 0 si les chaines sont identiques
        // s1 = première chaine de caractère
        // s2 = deuxième chaine de caractère
        // n = nombre de caractère à comparer
        if(strncmp(msg, "sudo", 4) == 0){

            // Définis un protocol pour les commandes
            // Crée des tokens à partir du message reçu
            // Premier token = sudo
            // Deuxième token = commande
            // Troisième token = argument (optionnel)
            // Quatrième token = message (optionnel)

            // Permet de découper une chaine de caractère en plusieurs tokens
            // char* strtok(char *s, const char *delim)
            // Renvoie un pointeur sur le premier token trouvé
            // s = chaine de caractère à découper, NULL pour continuer à découper la chaine précédente
            // delim = caractère de séparation
            
            // Inutile de stocker le pointeur retourné, car on sait que le premier token est "sudo"
            strtok(msg, " ");

            char* commande = strtok(NULL, " ");
            
            char* arg;
            if(strncmp(commande, "mp", 2) == 0 || strncmp(commande, "kick", 4) == 0 || strncmp(commande, "rename", 6) == 0){
                arg = strtok(NULL, " ");
                if(arg == NULL || strcmp(arg, "") == 0){
                    // Avertis le client que la commande est mal formulée
                    char errorMsg[MAX_CHAR] = "\033[41m[ERROR]\033[0m La commande est mal formulée";
                    send(getDSC(i), errorMsg, strlen(errorMsg) + 1, 0); 
                    continue;
                }
            }
            
            char* message;
            if(strncmp(commande, "all", 3) == 0 || strncmp(commande, "mp", 2) == 0){
                message = strtok(NULL, "\0");
                if(message == NULL || strcmp(message, "") == 0){
                    // Avertis le client que la commande est mal formulée
                    char errorMsg[MAX_CHAR] = "\033[41m[ERROR]\033[0m La commande est mal formulée";
                    send(getDSC(i), errorMsg, strlen(errorMsg) + 1, 0); 
                    continue;
                }
            }

            // Vérifie si la commande est "sudo help"
            if(strncmp(commande, "all", 3) == 0){
                // Envoie le message à tous les clients
                sendAll(message, getDSC(i));
            }
            // Vérifie si la commande est "sudo mp <pseudo> <msg>"
            else if(strncmp(commande, "mp", 2) == 0){
                if(getDSCByPseudo(arg) == -1){
                    // Avertis le client que le client <pseudo> n'existe pas
                    char errorMsg[MAX_CHAR] = "Le client n'existe pas";
                    send(getDSC(i), errorMsg, strlen(errorMsg) + 1, 0); 
                    continue;
                }
                // Envoie le message au client <pseudo>
                sendMP(message, i, getDSCByPseudo(arg)); 
            }
            // Vérifie si la commande est "sudo help"
            else if(strncmp(commande, "help", 4) == 0){
                // Envoie la liste des commandes au client
                sendHelp(getDSC(i));
            }
            // Vérifie si la commande est "sudo quit"
            else if(strncmp(commande, "quit", 4) == 0){
                // Envoie un message de déconnexion au client
                sendQuit(getDSC(i));
            }
            // Vérifie si la commande est "sudo list"
            else if(strncmp(commande, "list", 4) == 0){
                // Envoie la liste des clients au client
                sendList(getDSC(i));
            }
            // Vérifie si la commande est "sudo kick <pseudo>"
            else if(strncmp(commande, "kick", 4) == 0){
                if(getDSCByPseudo(arg) == -1){
                    // Avertis le client que le client <pseudo> n'existe pas
                    char errorMsg[MAX_CHAR] = "Le client n'existe pas";
                    send(getDSC(i), errorMsg, strlen(errorMsg) + 1, 0);
                }
                else{
                    // Envoie un message de déconnexion au client <pseudo>
                    sendKick(i, getDSCByPseudo(arg));   
                }
            }
            // Vérifie si la commande est "sudo rename <pseudo>"
            else if(strncmp(commande, "rename", 6) == 0){
                // Change le pseudo du client
                sendRename(arg, getDSC(i));
            }
            else{
                // Avertis le client que la commande n'existe pas
                char* errorMsg = "\033[41m[ERROR]\033[0m Commande inconnue, tapez \"sudo help\" pour afficher la liste des commandes";
                send(getDSC(i), errorMsg, strlen(errorMsg) + 1, 0);
            }
        }
        else{
            // Envoie le message reçu à tous les clients (commande par défaut)
            sendAll(msg, getDSC(i));
        }
    }

    // Le threadMemory va gérer la fermeture des threads
    // On fait ceci pour éviter un warning et pour éviter de fermer le thread ici
    while(1){
        printf("\033[36m[INFO]\033[0m Un thread est en attente de fermeture\n");
        sleep(1);
    }

    // Ne s'exécute jamais
    pthread_exit(0);
}

// Fonction permettant de gérer la fermeture des threads
void * threadMemory(){
    while(1){
        sem_wait(&semaphoreMemory);
        pthread_mutex_lock(&mutex);
        for(int i = 0; i < MAX_CLIENT; i++){
            if(threadEnd[i] == 1){
                
                // Rend le descripteur de socket disponible
                dSCList[i] = -1;

                // Réinitialise le pseudo du client
                sprintf(pseudoList[i], "Client%d", i);

                // Ferme le thread
                // int pthread_cancel(pthread_t thread);
                // Renvoie 0 si le thread est fermé, sinon renvoie une erreur
                // thread = identifiant du thread à fermer
                if(pthread_cancel(threadC[i]) != 0){
                    perror("Erreur fermeture thread\n");
                    exit(0);
                }

                threadEnd[i] = 0;

                // Incrémente le nombre de place disponible dans le semaphore
                sem_post(&semaphoreSlot);
            }
        }
        pthread_mutex_unlock(&mutex);
    }
    pthread_exit(0);
}

// Fonction de gestion des signaux
void sigint_handler(int sig) {

    printf("\n\033[36m[INFO]\033[0m Signal CTRL+C reçu, fermeture du serveur\n");

    // Ferme la connexion du serveur
    // int shutdown(int dS, int mode)
    // Renvoie 0 si la fermeture est réussi et -1 si elle échoue
    // dS = descripteur de socket
    // 2 = fermeture de la connexion
    shutdown(dS, 2);

    exit(0);
}


// Fonction principale
int main(int argc, char *argv[]) {

    // Vérification du nombre d'argument
    // argv[0] = port du serveur
    if(argc != 2){
        printf("\033[41m[ERROR]\033[0m Erreur nombre d'argument\n");
        exit(0);
    }

    // Initialise la liste de structure d'adresse de socket
    initAC();

    // Gestion des signaux
    signal(SIGINT, sigint_handler);

    // Initialise la liste des descripteurs de socket des clients à -1
    initDSCList();

    // Initialise les pseudos des clients à l'id du client
    initPseudoList();

    // Initialise la liste des threads à 0
    initThreadEnd();

    // Initialise les semaphores
    // int sem_init(sem_t *sem, int pshared, unsigned int value);
    // Renvoie 0 en cas de succès et -1 en cas d'échec
    // sem = pointeur sur le semaphore
    // pshared = 0 si le semaphore est partagé entre les threads d'un même processus, 1 si le semaphore est partagé entre les processus
    // value = valeur initiale du semaphore
    if(sem_init(&semaphoreSlot, 0, MAX_CLIENT) == -1){
        perror("Erreur initialisation semaphore\n");
        exit(0);
    }

    if(sem_init(&semaphoreMemory, 0, 0) == -1){
        perror("Erreur initialisation semaphore\n");
        exit(0);
    }


    // Création de la structure aS
    // aS = adresse du serveur
    struct sockaddr_in aS;

    // Stocke la famille d'adresse dans la structure aS
    // AF_INET = Protocole IP
    aS.sin_family = AF_INET;

    // Converti l'adresse IP en format réseau
    // INADDR_ANY = adresse IP de la machine
    aS.sin_addr.s_addr = INADDR_ANY ;

    // Converti le port en format réseau
    // unsigned short htons(unsigned short hostshort)
    // Renvoie le port en format réseau en cas de succès et 0 en cas d'échec
    // argv[1] = port du serveur
    aS.sin_port = htons(atoi(argv[1])) ;

    // Création du socket pour les clients
    // int socket(int domaine, int type, int protocole)
    // Renvoie le descripteur de socket si la création est réussi et -1 si elle échoue
    // dS = descripteur de socket
    // PF_INET = Protocole IP
    // SOCK_STREAM = Protocole TCP
    // 0 = Protocole par défaut
    dS = socket(PF_INET, SOCK_STREAM, 0);
    
    // Le socket est créé 

    // Liaison de la socket au serveur
    // int bind(int dS1, struct sockaddr *aS, socklen_t lgA)
    // Renvoie 0 si la liaison est réussi et -1 si elle échoue
    // dS1 = descripteur de socket
    // &aS = adresse du serveur
    // sizeof(aS) = taille de la structure aS
    bind(dS, (struct sockaddr*)&aS, sizeof(aS));

    // Le socket est nommé

    // Mise en écoute de la socket
    // int listen(int dS, int nbC)
    // Renvoie 0 si la mise en écoute est réussi et -1 si elle échoue
    // dS = descripteur de socket
    // nbC = nombre de connexion en attente
    listen(dS, 2);


    // Création du thread pour la gestion de la mémoire partagée pour les autres threads
    pthread_t threadClean;
    pthread_create(&threadClean, NULL, threadMemory, NULL);

    // Gestion des clients
    // Dès qu'un client se connecte, un thread est créé pour gérer la connexion

    printf("\033[36m[INFO]\033[0m Serveur lancé\n");

    
    while(1) {

        // Bloque le thread tant qu'il n'y a pas de place
        // int sem_wait(sem_t *sem);
        // Renvoie 0 en cas de succès et -1 en cas d'échec
        // sem = pointeur sur le semaphore
        if(sem_wait(&semaphoreSlot) == -1){
            perror("Erreur lors de l'attente du semaphore\n");
            exit(0);
        }

        for(int i = 0; i < MAX_CLIENT; i++){
            if(dSCList[i] == -1){
                // Accepte la connexion du client
                // int accept(int dS, struct sockaddr *aC, socklen_t *lg)
                // Renvoie le descripteur de socket du client
                // dS = descripteur de socket
                // &aC = adresse du client
                // &lg = l'adresse de la taille de la structure aC
                if((dSCList[i] = accept(dS, (struct sockaddr*) &aC[i], &lg[i])) == -1){
                    perror("accept");
                    exit(0);
                }

                pthread_mutex_lock(&mutex);
                pthread_create(&threadC[i], NULL, clientReceive, (void *) (long) i);
                pthread_mutex_unlock(&mutex);
            }
        }
    }

    // Ferme la connexion du serveur
    // int shutdown(int dS1, int mode)
    // Renvoie 0 si la fermeture est réussi et -1 si elle échoue
    // dS1 = descripteur de socket
    // 2 = fermeture de la connexion
    shutdown(dS, 2);

    printf("\033[36m[INFO]\033[0m Le serveur est fermé\n");

    exit(0);
}