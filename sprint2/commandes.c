void sendToAll(char* msg, int dSC){
    for(int i = 0; i < MAX_CLIENT; i++){
        if(dSCList[i] != dSC && dSCList[i] != -1){
            //Envoie un message au client i
            //int send(int dSC, void *msg, int lg, int flags)
            //Renvoie le nombre d'octet envoyé
            //dSC = descripteur de socket du client
            //&recvR = message envoyé
            //sizeof(int) = taille du message (ici 4)
            //0 = flags
            send(dSCList[i], msg, MAX_MSG, 0);
        }
    }
}