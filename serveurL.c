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

    int dS = socket(PF_INET, SOCK_STREAM, 0);
    printf("Socket Créé\n");


    struct sockaddr_in ad;
    ad.sin_family = AF_INET;
    ad.sin_addr.s_addr = INADDR_ANY ;
    ad.sin_port = htons(atoi(argv[1])) ;
    bind(dS, (struct sockaddr*)&ad, sizeof(ad)) ;
    printf("Socket Nommé\n");

    listen(dS, 7) ;
    printf("Mode écoute\n");

    struct sockaddr_in aC ;
    socklen_t lg = sizeof(struct sockaddr_in) ;
    int dSC = accept(dS, (struct sockaddr*) &aC,&lg) ;
        printf("Client Connecté\n");

    char msg1[32];
    char msg2[32];

    int recvR1 = recv(dSC, msg1, sizeof(msg1), 0) - 1;
    int recvR2 = recv(dSC, msg2, sizeof(msg2), 0) - 1;

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


    send(dSC, &recvR, sizeof(int), 0);
    printf("Message Envoyé\n");
    shutdown(dSC, 2); 
    shutdown(dS, 2);
    printf("Fin du programme\n");
}