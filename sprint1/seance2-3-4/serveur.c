#include <stdio.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>

// Relaye les messages du client 1 au client 2
void * C1toC2(void* dSC){
    
    int dSC1 = ((int*) dSC)[0];
    int dSC2 = ((int*) dSC)[1];

    char msg[255];
    int fin = 0;

    while (!fin) {

        //Reçoit un message du client 1
        //int recv(int dSC, void *msg, int lg, int flags)
        //Renvoie le nombre d'octet reçu
        //dSC = descripteur de socket du client
        //msg = message reçu
        //sizeof(msg) = taille du message (ici 32)
        //0 = flags
        int recvC1 = recv((int) dSC1, msg, sizeof(msg), 0) - 1;

        //Vérifie si la connexion est interrompu ou si une erreur est survenue
        if(recvC1 == 0 || recvC1 == -1){
            perror("Erreur interruption de la connexion ou erreur du client 1");
            break;
        }
        else if (strcmp(msg, "fin") == 0) {
            fin = 1;
            printf("Le client 1 ferme la connexion\n");
        }

        printf("Message reçu : %s\n", msg);

        //Envoie un message au client 2
        //int send(int dSC, void *msg, int lg, int flags)
        //Renvoie le nombre d'octet envoyé
        //dSC = descripteur de socket du client
        //&recvR = message envoyé
        //sizeof(int) = taille du message (ici 4)
        //0 = flags
        send((int) dSC2, msg, sizeof(msg), 0);

        printf("Message envoyé\n");
    }
}

// Relaye les messages du client 2 au client 1
void * C2toC1(void* dSC1, void* dSC2){
    char msg[255];
    int fin = 0;

    while (!fin) {
        //Reçoit un message du client 2
        int recvC2 = recv((int) dSC2, msg, sizeof(msg), 0) - 1;

        //Vérifie si la connexion est interrompu ou si une erreur est survenue
        if(recvC2 == 0 || recvC2 == -1){
            perror("Erreur interruption de la connexion ou erreur du client 2");
            break;
        }
        else if (strcmp(msg, "fin") == 0) {
            fin = 1;
            printf("Le client 2 ferme la connexion\n");
        }

        printf("Message reçu : %s\n", msg);

        //Envoie un message au client 1
        send((int) dSC1, msg, sizeof(msg), 0);

        printf("Message envoyé\n");
    }
}




int main(int argc, char *argv[]) {

    //Vérification du nombre d'argument
    //argv[0] = port du serveur
    if(argc != 2){
        printf("Erreur nombre d'argument\n");
        exit(0);
    }

    printf("Début programme\n");
    
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

    while(1){

        //Création de la structure aC
        //aC = adresse du client
        struct sockaddr_in aC1;
        struct sockaddr_in aC2;

        //Stocke la taille de la structure aC dans lg
        socklen_t lg1 = sizeof(struct sockaddr_in);
        socklen_t lg2 = sizeof(struct sockaddr_in);

        //Accepte la connexion du client
        //int accept(int dS, struct sockaddr *aC, socklen_t *lg)
        //Renvoie le descripteur de socket du client
        //dS = descripteur de socket
        //&aC = adresse du client
        //&lg = l'adresse de la taille de la structure aC
        int dSC1 = accept(dS, (struct sockaddr*) &aC1,&lg1);
        printf("Le client 1 est connecté\n");

        int dSC2 = accept(dS, (struct sockaddr*) &aC2,&lg2);
        printf("Le client 2 est connecté\n");

        //Création de thread pour client 1 au client 2
        //Et client 2 au client 1
        
        //pthread_t est un type de donnée qui représente un thread
        pthread_t threadC1ToC2;
        pthread_t threadC2ToC1;

        //Création d'un tableau de descripteur de socket 
        //Pour les passer en paramètre des thread
        int* dSC = malloc(2*sizeof(int));
        dSC[0] = dSC1;
        dSC[1] = dSC2;

        //creation des thread
        pthread_create(&threadC1ToC2, NULL, C1toC2, (void *) dSC);
        pthread_create(&threadC2ToC1, NULL, C2toC1, (void *) dSC);

        printf("Fin de la conversation\n");

        //Ferme la connexion du client
        //int shutdown(int dSC, int mode)
        //Renvoie 0 si la fermeture est réussi et -1 si elle échoue
        //dSC = descripteur de socket du client
        //2 = fermeture de la connexion
        shutdown(dSC1, 2); 
        shutdown(dSC2, 2);

        printf("Nouvelle conversation possible, en attente de client\n");

    }

    //Ferme la connexion du serveur
    //int shutdown(int dS1, int mode)
    //Renvoie 0 si la fermeture est réussi et -1 si elle échoue
    //dS1 = descripteur de socket
    //2 = fermeture de la connexion
    shutdown(dS, 2);

    printf("Fin du programme\n");
}