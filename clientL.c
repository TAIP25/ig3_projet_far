    #include <stdio.h>
    #include <sys/socket.h>
    #include <arpa/inet.h>
    #include <stdlib.h>
    #include <string.h>

int main(int argc, char *argv[]) {


    //Vérification du nombre d'argument
    //argv[0] = ip du serveur
    //argv[1] = port du serveur
    if(argc != 3){
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

    //Création de la structure aS
    //aS = adresse du serveur
    struct sockaddr_in aS;

    //Stocke la famille d'adresse dans la structure aS
    //AF_INET = Protocole IP
    aS.sin_family = AF_INET;

    //Converti l'adresse IP en format réseau
    //INADDR_ANY = adresse IP de la machine
    //argv[1] = adresse IP du serveur
    //&aS.sin_addr = adresse du serveur
    inet_pton(AF_INET,argv[1],&(aS.sin_addr));

    //Converti le port en format réseau
    //argv[2] = port du serveur
    aS.sin_port = htons(atoi(argv[2]));

    //Stocke la taille de la structure aS dans lgA
    socklen_t lgA = sizeof(struct sockaddr_in);

    //Connecte la socket au serveur
    //dS = descripteur de socket
    //aS = adresse du serveur
    //lgA = taille de la structure aS
    connect(dS, (struct sockaddr *) &aS, lgA);

    printf("Socket Connecté\n");

    int nbMessage = 2;
    while(nbMessage != 0){
            
        char m[32];
        printf("Entrez votre message : ");
        scanf("%s", m);
        if(strlen(m) > 32){
            printf("Erreur message trop grand\n");
        }
        else{

            //Envoie le message au serveur
            //dS = descripteur de socket
            //m = message
            //strlen(m) + 1 = taille du message
            //0 = protocole par défaut
            send(dS, m, strlen(m) + 1 , 0);

            nbMessage -= 1;
            printf("Message Envoyé, il te reste %d message(s)\n", nbMessage);
        }
    }




    int r;

    //Reçoit la réponse du serveur
    //dS = descripteur de socket
    //&r = réponse
    //sizeof(int) = taille de la réponse
    //0 = protocole par défaut
    recv(dS, &r, sizeof(int), 0) ;

    printf("Réponse reçue (nb octets): %d\n", r) ;

    //Ferme la connexion
    //dS = descripteur de socket
    //2 = fermeture des deux sens
    shutdown(dS,2) ;
    
    printf("Fin du programme\n");
}