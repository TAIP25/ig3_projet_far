# Livrable du sprint 2

## Table des matières

## Arborecense du sprint

## Description du sujet

## Difficultés rencontrées

Nous avons rencontré des difficultés par rapport au découpage des commandes en token: seul le premier mot entré été renvoyé au destinataire pour la commande message privé. 
Ceci a vite été résolu en revoyant le code.
Puis nous avons eu quelque difficulté pour les commandes "sudo quit" et "sudo kick" pour la fermeture de manière propre de la connexion client et que côté client, il gère cette fermeture.
Enfin nous avons surtout eu des difficultés pour comprendre comment, pourquoi et dans quel cas il fallait employer les mutex, sémaphores et mémoire partagée.
Nous avons donc dû faire des recherches et des tests pour comprendre comment les utiliser et les implémenter dans notre code.
Par exemple les mutex nous avons fait le choix dans un premier temps de l'utilisé que pour l'écriture des variables partagées.
Puis nous avons décidé de l'utiliser aussi pour la lecture des variables partagées, fallait t'il en faire pour chaque variables partagées ou un seul pour tous, etc ...
Enfin nous avons eu des difficultés pour la fermeture des threads avec la mémoire partagée, car nous avons dû faire des tests pour comprendre comment cela fonctionnait et comment l'implémenter dans notre code.

## Répartition du travail

Tout d'abord avant de coder, nous avons décidé en amont comment chaque étape clé devrait être traitée pour donner des indications claires à chacun de nous.
Vu que le travail en parallèle a bien fonctionné le dernier sprint, nous sommes restés sur la même optique.
Puis la répartition du travail s'est faite de manière à pouvoir travailler continuellement sur un même aspect du projet et à pouvoir respecter les délais imposés.
Pour ce sprint, nous avons déterminé que la partie client ne nécessitait pas de grands changements, donc nous avons tous les deux travaillé sur la partie serveur.
Après s'être mis d'accord sur les commandes utilisables (voir fichier commandes.txt), Léon s'est occupé de la réorganisation des fichiers, implémentation de mutex, sémaphores, synchronisation des threads.
En parallèle, Wayne s'est occupé de la gestion des signaux et des commandes: détection, découpage en token et implémentation des fonctions. Il a été important de travailler au même moment l'un a côté de l'autre en cas de blocage et d'incertitude sur la manière de procéder.
Cependant, ce sprint étant plus long et avec l'impossibilité de se voir pendant les vacances scolaires, la fin de ce sprint par appel vocal sur discord.
Enfin, nous avons fait des test chacun de notre côté pour les améliorations possibles.

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


