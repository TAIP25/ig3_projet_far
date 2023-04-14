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
    //int socket(int domaine, int type, int protocole)
    //Renvoie le descripteur de socket si la création est réussi et -1 si elle échoue
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
    //int inet_pton(int af, const char *src, void *dst)
    //Renvoie 1 si la conversion est réussi et 0 si elle échoue
    //INADDR_ANY = adresse IP de la machine
    //argv[1] = adresse IP du serveur
    //&aS.sin_addr = adresse du serveur
    inet_pton(AF_INET,argv[1],&(aS.sin_addr));

    //Converti le port en format réseau
    //unsigned short htons(unsigned short hostshort)
    //Renvoie le port en format réseau en cas de succès et 0 en cas d'échec
    //argv[2] = port du serveur
    aS.sin_port = htons(atoi(argv[2]));

    //Stocke la taille de la structure aS dans lgA
    socklen_t lgA = sizeof(struct sockaddr_in);

    //Connecte la socket au serveur
    //int connect(int dS, struct sockaddr *aS, socklen_t lgA)
    //Renvoie 0 si la connexion est réussi et -1 si elle échoue
    //dS = descripteur de socket
    //aS = adresse du serveur
    //lgA = taille de la structure aS
    connect(dS, (struct sockaddr *) &aS, lgA);

    printf("Socket Connecté\n");

    //Envoie le nombre de message au serveur
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
            //int send(int dS, const void *m, size_t lg, int flags)
            //Renvoie le nombre d'octet envoyé si la connexion est réussi et -1 si elle échoue
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
    //int recv(int dS, void *r, size_t lg, int flags)
    //Renvoie le nombre d'octet reçu si la connexion est réussi et -1 si elle échoue
    //dS = descripteur de socket
    //&r = réponse
    //sizeof(int) = taille de la réponse
    //0 = protocole par défaut
    recv(dS, &r, sizeof(int), 0) ;

    printf("Réponse reçue (nb octets): %d\n", r) ;

    //Ferme la connexion
    //int shutdown(int dS, int how)
    //Renvoie 0 si la fermeture est réussi et -1 si elle échoue
    //dS = descripteur de socket
    //2 = fermeture des deux sens
    shutdown(dS,2) ;
    
    printf("Fin du programme\n");
}