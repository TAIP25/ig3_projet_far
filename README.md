# Livrable du projet far 2022-2023: Application de messagerie intantanée

Voici le projet far de **Léon BOUDIER et Wayne WAN CHOW WAH**. Ce projet nous a permis de bien comprendre les concepts du IPC, mutex, sémaphores, protocoles. Le but final de ce projet est de créer une messagerie instantanée avec des salons où plusieurs utilisateurs peuvent se connecter et avoir une conversation en live. Ce n'est cependant pas qu'une simple messagerie de communication: de nombreuses commandes sont disponibles (se référer au fichier commandes.txt) pour laisser l'utilisateur libre pensée à ce qu'il peut faire dans la conversation. Il peut s'amuser à virer des gens du serveur, les bouger de salon, envoyer des messages privés, et meme devenir superadmin pour avoir plus de commandes.

Le projet est découpé en sprint avec son code et son livrable pour chaque sprint. Ce README est le livrable final en vue de la soutenance. Il regroupe une explication brève de ce que nous avons accompli après chaque sprint pour voir une progression des caractéristiques inédits de notre projet.

## Table des matières


## Prérequis

### Mermaid

Pour afficher les différents diagrammes de séquence, **il faut installer l'extension mermaid pour visual studio code**. Si vous vous trouvez sur github, affichage est directement géré par celui-ci.

## Arborecence



## Sprints

Le projet a été fait en 4 sprints distincts. Dans chaque nouveau sprint, le but a été d'améliorer la messagerie en ajoutant des fonctionnalités typiques à une messagerie.

## Sprint1

### Au global
Mise en place d’un serveur qui puisse gérer n clients, pour cela mettre en place un tableau partagé pour stocker leurs identifiants sockets, par défaut, les messages arrivant depuis un client sont relayés à tous les autres présents.

### Au niveau du serveur
1 thread par client pour écouter les messages qui proviennent du client et les diffuser vers tous les autres clients. 

### Au niveau du client
1 processus pour la saisie (avec fgets) et l’envoi du message au serveur et 1 autre processus pour la réception des messages du serveur et leur affichage (avec puts) soit le programme principal plus un thread minimum.

Pour plus de détail sur le fonctionnement de la messagerie du sprint1, se référer à son README disponible dans le fichier sprint1.


## Sprint2

### Au global
Le serveur peut toujours gérer n clients. Nous avons fait de nouveaux fichiers pour séparer les commandes et les variables globales afin de mieux se repérer dans les fichiers. L'implémentation de mutex, de sémaphores, gestion de signaux et de gestion des commandes en par découpage en token a été clé pour cette partie.  Nous avons également défini un protocole pour les commandes particulières envoyées depuis le client, permettant ainsi d'ajouter des fonctionnalités spécifiques telles que les messages privés. Pour les messages privés, nous avons implémenté une commande qui permet à un client d'envoyer un message à un autre client en utilisant un numéro de client ou un identifiant, et nous avons ajouté la possibilité d'utiliser un pseudo pour les échanges privés.

### Au niveau du serveur
Du côté du serveur, nous avons mis en place une gestion des erreurs pour vérifier l'existence du destinataire et éviter les doublons de pseudonymes. De plus, nous avons créé une commande de déconnexion qui permet au client de se déconnecter sans affecter les autres clients. Nous avons également effectué des améliorations du code, notamment en ajoutant un mutex pour le tableau des clients, un sémaphore pour indiquer le nombre de places restantes sur le serveur, et en gérant les signaux (Ctrl+C) pour une fermeture propre des threads lors de la déconnexion des clients et de la connexion de nouveaux clients. Enfin, nous avons ajouté une commande permettant de lister les fonctionnalités disponibles pour le client, stockées dans un fichier texte.

### Au niveau du client
Pas de grand changement.

Pour plus de détail sur le fonctionnement de la messagerie du sprint2, se référer à son README disponible dans le fichier sprint2.


## Sprint3

Nous avons créé une commande permettant à un client d'envoyer un fichier. À tout moment, le client peut saisir cette commande et choisir parmi une liste de fichiers d'un répertoire dédié à l'application. Le fichier sélectionné est ensuite envoyé au serveur, qui l'enregistre dans un répertoire spécifique. Le client envoie préalablement au serveur le nom et la taille du fichier pour assurer une transmission correcte. Cette commande déclenche un nouveau processus à la fois côté client et côté serveur pour gérer l'envoi du fichier. Un canal de communication parallèle est établi avec un nouveau socket, une nouvelle connexion et un nouveau port sur le serveur pour les messages liés à l'envoi du fichier. Nous avons amélioré cette fonctionnalité en affichant la liste des fichiers disponibles dans le dossier de partage côté client, et chaque fichier est associé à un numéro pour faciliter la sélection.

En plus de l'envoi de fichier, nous avons ajouté une commande permettant à un client de récupérer des fichiers disponibles sur le serveur. Le serveur crée un nouveau socket et un nouveau thread pour l'envoi du fichier, afin de ne pas utiliser les canaux de messagerie existants. Le client dispose également d'un thread dédié à la réception du fichier, ce qui lui permet de continuer à échanger des messages textuels pendant le processus de récupération.

Enfin, nous avons mis à jour le manuel en ajoutant les nouvelles commandes afin que les utilisateurs aient toutes les informations nécessaires sur les fonctionnalités disponibles dans l'application.

Pour plus de détail sur le fonctionnement de la messagerie du sprint3, se référer à son README disponible dans le fichier sprint3.


## Sprint4

Pour plus de détail sur le fonctionnement de la messagerie du sprint4, se référer à son README disponible dans le fichier sprint4.

