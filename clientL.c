    #include <stdio.h>
    #include <sys/socket.h>
    #include <arpa/inet.h>
    #include <stdlib.h>
    #include <string.h>

int main(int argc, char *argv[]) {

    printf("Début programme\n");
    int dS = socket(PF_INET, SOCK_STREAM, 0);
    printf("Socket Créé\n");

    struct sockaddr_in aS;
    aS.sin_family = AF_INET;
    inet_pton(AF_INET,argv[1],&(aS.sin_addr)) ;
    aS.sin_port = htons(atoi(argv[2])) ;
    socklen_t lgA = sizeof(struct sockaddr_in) ;
    connect(dS, (struct sockaddr *) &aS, lgA) ;
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
            send(dS, m, strlen(m) + 1 , 0);

            nbMessage -= 1;
            printf("Message Envoyé, il te reste %d message(s)\n", nbMessage);
        }
    }




    int r;
    recv(dS, &r, sizeof(int), 0) ;
    printf("Réponse reçue (nb octets): %d\n", r) ;

    shutdown(dS,2) ;
    printf("Fin du programme\n");
}