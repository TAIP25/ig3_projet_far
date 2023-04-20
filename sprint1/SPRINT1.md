# projet_far


i) le protocole de communication entre les clients et le(s) serveurs (diagramme de séquence 	UML),


ii) le cas échéant (si le protocole ne suffit pas), l’architecture de l’application (en termes de processus, threads, ressources/données partagées, mécanismes de synchronisation utilisés, ...), 


iii) les difficultés rencontrées, les choix faits pour les surmonter, et les problèmes qui 	persistent,


iv) la répartition du travail entre les membres du groupe (très important, pour montrer la bonne organisation de l’équipe),

La répartition du travail s'est faite de manière à pouvoir travailler continuellement sur un même aspect du projet et à pouvoir respecter les délais imposés. Lorsque Léon travaille sur la partie serveur, Wayne travaille en parallèle sur la partie client pour une meilleure efficacité et rapidité. Biensur, après chaque étape, il faut réunir les résultats et s'assurer que les concepts et les méthodes utilisées sont correctes. Un point de vue critique est alors apporté au moment où Wayne revoit la partie du serveur, et Léon revoit la partie du client.  

Il a été important de travailler au même moment l'un a coté de l'autre en cas de blocage et d'incertitude sur la manière de procéder. 

Pour nous organiser nous avons séparé le projet en 3 parties qui sont séparés dans le fichier SPRINT1. 

Partie 1: sujet de la séance 1: Un serveur relaie des messages textuels entre deux clients
Partie 2: sujet de la séance 2 (v1): Utiliser le multi-threading pour gérer l’envoi de messages dans n’importe quel ordre
Partie 3: sujet de la séance 2: (v2): Mise en place d’un serveur qui puisse gérer n clients

Les détails du procédé de chaque partie est détaillé dans leur fichier où un README est disponible. 



v) comment compiler et exécuter le code (commandes gcc, make, ...).

Pour compiler et exécuter le code, il suffit de se situer au bon endroit dans le fichier du SPRINT1. Chaque partie (1, 2 et 3) est localisé dans son fichier. 

Pour lancer le programme, voici les commandes à effectuer dans un terminal:

Partie 1
- d'abord compiler le fichier serveur pour avoir un fichier C compilable: gcc -Wall -o serveur serveur.c
- ensuite lancer le serveur: ./serveur <port du serveur choisi>  
    exemple: ./serveur 5000
    NB: après chaque lancement de serveur, il faudra utiliser un autre port pour lancer un nouveau serveur
- puis compiler le fichier client: gcc -Wall -o client client.c
- et lancer le client: ./client 127.0.0.1 <port du serveur>
    exemple avec le serveur de port 5000: ./client 127.0.0.1 5000
    NB: il faut que le port du client et du serveur soient le même
  
Partie 2
- d'abord compiler le fichier serveur pour avoir un fichier C compilable: gcc -Wall -o serveur serveur.c
- ensuite lancer le serveur: ./serveur <port du serveur choisi>
    exemple: ./serveur 5000
    NB: après chaque lancement de serveur, il faudra utiliser un autre port pour lancer un nouveau serveur
- puis compiler le fichier client: gcc -Wall -o client client.c
- et lancer le client1: ./client 127.0.0.1 <port du serveur> <numéro du client>
    exemple avec le serveur de port 5000: ./client 127.0.0.1 5000 1
- et lancer le client2: ./client 127.0.0.1 <port du serveur> <numéro du client>
    exemple avec le serveur de port 5000: ./client 127.0.0.1 5000 2
    NB: il faut que le port du client et du serveur soient le même
  
Partie 3
- d'abord compiler le fichier serveur pour avoir un fichier C compilable: gcc -Wall -o serveur serveur.c
- ensuite lancer le serveur: ./serveur <port du serveur choisi>
    exemple: ./serveur 5000
    NB: après chaque lancement de serveur, il faudra utiliser un autre port pour lancer un nouveau serveur
- puis compiler le fichier client: gcc -Wall -o client client.c
- et lancer pour chaque client: ./client 127.0.0.1 <port du serveur> 
    exemple avec le serveur de port 5000: ./client 127.0.0.1 5000 
    NB: il faut que le port du client et du serveur soient le même
    NB: chaque client sera identifié sur le serveur automatiquement
