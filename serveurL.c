    #include <stdio.h>
    #include <sys/socket.h>
    #include <arpa/inet.h>
    #include <stdlib.h>
    #include <string.h>

int main(int argc, char *argv[]) {

    //Vérification du nombre d'argument
    //argv[0] = port du serveur
    if(argc != 2){
        printf("Erreur nombre d'argument\n");
        exit(0);
    }

    printf("Début programme\n");

    //Création de la socket
    //dS = descripteur de socket
    //PF_INET = Protocole IP
    //SOCK_STREAM = Protocole TCP
    //0 = Protocole par défaut
    int dS = socket(PF_INET, SOCK_STREAM, 0);
    printf("Socket Créé\n");

    //Création de la structure aD
    //aD = adresse du serveur
    struct sockaddr_in aD;

    //Stocke la famille d'adresse dans la structure aD
    //AF_INET = Protocole IP
    aD.sin_family = AF_INET;

    //Converti l'adresse IP en format réseau
    //INADDR_ANY = adresse IP de la machine
    aD.sin_addr.s_addr = INADDR_ANY ;

    //Converti le port en format réseau
    //argv[1] = port du serveur
    aD.sin_port = htons(atoi(argv[1])) ;

    //Liaison de la socket au serveur
    //int bind(int dS, struct sockaddr *aD, socklen_t lgA)
    //Renvoie 0 si la liaison est réussi et -1 si elle échoue
    //dS = descripteur de socket
    //&aD = adresse du serveur
    //sizeof(aD) = taille de la structure aD
    bind(dS, (struct sockaddr*)&aD, sizeof(aD)) ;

    printf("Socket Nommé\n");

    //Mise en écoute de la socket
    //int listen(int dS, int nbC)
    //Renvoie 0 si la mise en écoute est réussi et -1 si elle échoue
    //dS = descripteur de socket
    //7 = nombre de connexion en attente
    listen(dS, 7) ;

    printf("Mode écoute\n");

    //Création de la structure aC
    //aC = adresse du client
    struct sockaddr_in aC ;

    //Stocke la taille de la structure aC dans lg
    socklen_t lg = sizeof(struct sockaddr_in) ;

    //Accepte la connexion du client
    //int accept(int dS, struct sockaddr *aC, socklen_t *lg)
    //Renvoie le descripteur de socket du client
    //dS = descripteur de socket
    //&aC = adresse du client
    //&lg = l'adresse de la taille de la structure aC
    int dSC = accept(dS, (struct sockaddr*) &aC,&lg) ;
        printf("Client Connecté\n");

    char msg1[32];
    char msg2[32];

    //Reçoit un message du client
    //int recv(int dSC, void *msg, int lg, int flags)
    //Renvoie le nombre d'octet reçu
    //dSC = descripteur de socket du client
    //msg = message reçu
    //sizeof(msg) = taille du message (ici 32)
    //0 = flags
    int recvR1 = recv(dSC, msg1, sizeof(msg1), 0) - 1;
    int recvR2 = recv(dSC, msg2, sizeof(msg2), 0) - 1;

    //Vérifie si la connexion est interrompu ou si une erreur est survenue
    //Renvoie 0 si la connexion est interrompu et -1 si une erreur est survenue
    if(recvR1 == 0 || recvR1 == -1 || recvR2 == 0 || recvR2 == -1){
        perror("Erreur interruption de la connexion ou erreur");
        exit(0);
    }

    printf("Message reçu : %s\n", msg1);
    printf("Nb d'octet reçu : %d\n", recvR1);

    printf("Message reçu : %s\n", msg2);
    printf("Nb d'octet reçu : %d\n", recvR2);

    int recvR = recvR1 + recvR2; 
    printf("Nb d'octet totals reçu : %d\n", recvR);

    //Envoie un message au client
    //int send(int dSC, void *msg, int lg, int flags)
    //Renvoie le nombre d'octet envoyé
    //dSC = descripteur de socket du client
    //&recvR = message envoyé
    //sizeof(int) = taille du message (ici 4)
    //0 = flags
    send(dSC, &recvR, sizeof(int), 0);

    printf("Message Envoyé\n");

    //Ferme la connexion du client
    //int shutdown(int dSC, int mode)
    //Renvoie 0 si la fermeture est réussi et -1 si elle échoue
    //dSC = descripteur de socket du client
    //2 = fermeture de la connexion
    shutdown(dSC, 2); 

    //Ferme la connexion du serveur
    //int shutdown(int dS, int mode)
    //Renvoie 0 si la fermeture est réussi et -1 si elle échoue
    //dS = descripteur de socket
    //2 = fermeture de la connexion
    shutdown(dS, 2);
    
    printf("Fin du programme\n");
}