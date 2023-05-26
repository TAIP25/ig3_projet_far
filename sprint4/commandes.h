#ifndef COMMANDES_H
#define COMMANDES_H

// Vérifie si le client est connecté
// pre:
// post: isConnected(dSC) == 1 || isConnected(dSC) == 0
int isConnected(int dSC);

// Recupère la dSC du client à partir de son id
// pre: isConnected(dSC) == 1
// post: getDSC(getID(dSC)) == dSC
// post: getDSC(id) == dSC
// post: Attention, si le client n'est pas connecté, une erreur est throw
int getDSC(int id);

// Recupère l'id du client à partir de sa dSC
// pre: isConnected(dSC) == 1
// post: getID(getDSC(id)) == id
// post: getID(dSC) == id, 0 <= id < MAX_CLIENT
// post: Attention, si le client n'est pas connecté, une erreur est throw
int getId(int dSC);

// Recupère le pseudo du client à partir de sa dSC
// pre: isConnected(dSC) == 1
// post: getPseudo(dSC) == pseudo
// post: Attention, si le client n'est pas connecté, une erreur est throw
char* getPseudoByDSC(int dSC);

// Recupère le ID du client à partir de son pseudo
// pre: isConnected(dSC) == 1
// post: getID(getDSCByPseudo(pseudo)) == getIDByPseudo(pseudo) == id
// post: Attention, si le client n'est pas connecté => return -1
int getIDByPseudo(char* pseudo);

// Recupère le dSC du client à partir de son pseudo
// pre: isConnected(dSC) == 1
// post: getDSC(getIDByPseudo(pseudo)) == getDSCByPseudo(pseudo) == dSC
// post: Attention, si le client n'est pas connecté => return -1
int getDSCByPseudo(char* pseudo);

// Appelé quand le client envoie la commande "sudo all <msg>"
// Appelé aussi quand le client envoie "<msg>", car c'est la commande par défaut
// pre: isConnected(dSC) == 1
// post: Attention, si le client n'est pas connecté, une erreur est throw
// post: Attention, si l'envoie du message échoue, une erreur est throw
void sendAll(char* msg, int dSC);

// Appelé quand le client envoie la commande "sudo mp <id> <msg>"
// pre: isConnected(dSC) == 1
// post: Attention, si l'un des clients n'est pas connecté, une erreur est throw
// post: Attention, si l'envoie du message échoue, une erreur est throw
void sendMP(char* msg, int idS, int idR);

// Appelé quand le client envoie la commande "sudo help"
// pre: isConnected(dSC) == 1
// post: Attention, si le client n'est pas connecté, une erreur est throw
// post: Attention, si l'envoie du message échoue, une erreur est throw
void sendHelp(int dSC);

// Appelé quand le client envoie la commande "sudo quit"
// pre: isConnected(dSC) == 1
// post: Attention, si le client n'est pas connecté, une erreur est throw
// post: Attention, si l'envoie du message échoue, une erreur est throw
// post: Attention, si la fermeture de la connexion échoue, une erreur est throw
void sendQuit(int dSC);

// Appelé quand le client envoie la commande "sudo list"
// pre: isConnected(dSC) == 1
// post: Attention, si le client n'est pas connecté, une erreur est throw
// post: Attention, si l'envoie du message échoue, une erreur est throw
void sendList(int dSC);

// Appelé quand le client envoie la commande "sudo kick <id>"
// pre: isConnected(dSC) == 1
// post: Attention, si l'un des clients n'est pas connecté, une erreur est throw
// post: Attention, si l'envoie du message échoue, une erreur est throw
// post: Attention, si la fermeture de la connexion échoue, une erreur est throw
void sendKick(int idS, int idR);

// Vérifie si le pseudo est correct
// pre: pseudo != NULL
// post: Renvoie 1 si le pseudo est correct, 0 sinon
int properPseudo(char* pseudo);

// Appelé quand le client envoie la commande "sudo rename <pseudo>"
// pre: isConnected(dSC) == 1
// post: Attention, si le client n'est pas connecté, une erreur est throw
// post: Attention, si l'envoie du message échoue, une erreur est throw
void sendRename(char* pseudo, int dSC);

// Appelé quand le client envoie la commande "sudo download"
// pre: isConnected(dSC) == 1
// post: Attention, si le client n'est pas connecté, une erreur est throw
// post: Attention, si l'envoie du message échoue, une erreur est throw
void sendDownload(int dSC);

// Appelé quand le client envoie la commande "sudo ff15"
// pre: isConnected(dSC) == 1
// post: Attention, si le client n'est pas connecté, une erreur est throw
// post: Attention, si l'envoie du message échoue, une erreur est throw
void ff15(int dSC);

// Appelé quand le client envoie la commande "sudo create <roomName>"
// pre: isConnected(dSC) == 1
// post: Attention, si le client n'est pas connecté, une erreur est throw
// post: Si la room existe déjà, la commande est annulée
// post: Attention, si la création de la room échoue, une erreur est throw
void sendCreate(char* roomName, int dSC);

// Appelé quand le client envoie n'envoie pas de commande (commande par défaut)
// pre: isConnected(dSC) == 1
// post: Attention, si le client n'est pas connecté, une erreur est throw
// post: Attention, si l'envoie du message échoue, une erreur est throw
void sendRoom(char* msg, int dSC);

// Appelé quand le client envoie la commande "sudo join <roomName>" ou juste après qu'il crée une room
// pre: isConnected(dSC) == 1
// post: Attention, si le client n'est pas connecté, une erreur est throw
void sendJoin(char* roomName, int dSC);

// Appelé quand le client envoie la commande "sudo leave"
// pre: isConnected(dSC) == 1
// post: Attention, si le client n'est pas connecté, une erreur est throw
void sendLeave(int dSC);

// Appelé quand lorsque le client rejoins le serveur
// pre: isConnected(dSC) == 1
// post: Attention, si le client n'est pas connecté, une erreur est throw
void sendFirstJoin(int dSC);

// Appelé quand le client envoie la commande "sudo delete <roomName>"
// pre: isConnected(dSC) == 1
// post: Attention, si le client n'est pas connecté, une erreur est throw
void sendDelete(char* roomName, int dSC);

// Appelé quand le client envoie la commande "sudo move <roomName> <pseudo>"
// pre: isConnected(dSCS) && isConnected(dSCR) == 1
// post: Attention, si l'un des clients n'est pas connecté, une erreur est throw
void sendMove(char* roomName, int dSCS, int dSCR);

// Appelé quand le client envoie la commande "sudo superadmin <password>"
// pre: isConnected(dSC) == 1
// post: Attention, si le client n'est pas connecté, une erreur est throw
void sendSuperAdmin(char* password, int dSC);

#endif