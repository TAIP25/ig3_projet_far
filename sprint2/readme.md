# Livrable du sprint 2

## Table des matières

## Arborecense du sprint 

## Description du sujet

## Difficultés rencontrées

## Répartition du travail

Tout d'abord avant de coder, nous avons décidé en amont comment chaque étape clé devrait être traitée pour donner des indications claires à chacun de nous. Vu que le travail en parallèle a bien fonctionné le dernier sprint, nous sommes restés sur la même optique. Puis la répartition du travail s'est faite de manière à pouvoir travailler continuellement sur un même aspect du projet et à pouvoir respecter les délais imposés. Pour ce sprint, la partie client ne nécessitait pas de grand changements. Nous avons donc tous les deux travaillé sur la partie serveur. Après s'être mis d'accord sur les commandes utilisables (voir fichier commandes.txt), Léon s'est occupé de la réorganisation des fichiers, implémentation de mutex, sémaphores, synchronisation des threads. En parallèle, Wayne s'est occupé de la gestion des signaux et des commandes: détection, découpage en token et implémentation des fonctions. Il a été important de travailler au même moment l'un a côté de l'autre en cas de blocage et d'incertitude sur la manière de procéder, mais ce sprint étant plus long et l'impossibilité de se voir avec les vacances scolaires, le début a été fait de cette manière et la fin par appel vocal sur discord. Ainsi après chaque fin de partie 

## Compilation et exécution

### Tips

1. Port du serveur doit être supérieur à 1024 pour éviter certains problèmes

2. Pour lancer le serveur, il faut utiliser un autre port pour chaque lancement de serveur car cela peut créer des problèmes

3. Pour lancer les clients, il faut que le port du client et du serveur soit le même

4. Pour lancer le client, il faut que le serveur soit lancé avant le client

5. En local, l'adresse IP du serveur est 127.0.0.1

6. Possibilité de changer certaines valeurs comme le nombre de clients maximum, le nombre de caractères maximum par message, etc ...


### Pour executer:

1. D'abord compiler le fichier serveur pour avoir un fichier C compilable: 
        
        gcc global.c commandes.c serveur.c -Wall -o serveur

2. Ensuite lancer le serveur: 
        
        ./serveur <port du serveur choisi>  

3. Puis compiler le fichier client: 

        gcc global.c client.c -Wall -o client

4. Enfin pour lancer le client: 

        ./client <adresse IP du serveur> <port du serveur>


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

