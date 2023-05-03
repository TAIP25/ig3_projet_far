Pour executer:

    gcc global.c commandes.c serveur.c -Wall -o serveur
    gcc global.c client.c -Wall -o client

Info mutex
Mutex dans le main de serveur.c, dans la fonction quit, kick, rename (seule moment ou on modifie la liste des clients)

Amélioration:
Modifier help pour qu'il affiche via le .txt
(Ajout d’une commande permettant de lister les fonctionnalités disponibles pour le client, stockées dans un fichier texte ( manuel ).)

TODO:
Amélioration du code et gestion des nouveaux clients.
Ajout d’un sémaphore indiquant le nombre de place restante sur le serveur pour faciliter le remplacement de client et assurer un nombre de client maximum (  Utiliser la bibliothèque sys/sem.h : exemple ici ! )
https://www.tala-informatique.fr/wiki/index.php/C_semaphore
Gestion des signaux (Ctrl+C) client et serveur
Ajout d’une variable partagée pour une fermeture propre des threads lors de la déconnexion des clients et la connexion de nouveau clients
Synchronisation des threads des clients terminés

