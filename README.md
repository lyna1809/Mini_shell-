1 - Introduction: 
Ce projet est un mini-shell développé dans le cadre d'une introduction aux Systèmes . Il vise à reproduire les fonctionnalités essentielles d'un shell Unix standard, notamment l'exécution de commandes, la gestion des redirections, des pipes, des processus en arrière-plan et des signaux.


2 - Compilation et Exécution
Compilation :  make
Exécution :  ./shell


3 - Fonctionnalités Implémentées : 

1. Exécution de commandes simples : Le mini-shell exécute des commandes internes et externes :
          - Commandes internes (gérées sans création de processus) : cd, pwd, logout, echo, export, kill, jobs, bg, fg, stop .
          - Commandes externes (exécutées via fork() et execvp()) .

2. Gestion des redirections d’entrée et sortie : 
          - Redirection d’entrée (< fichier) : redirige STDIN_FILENO vers un fichier.
          - Redirection de sortie (> fichier) : redirige STDOUT_FILENO vers un fichier.

3. Exécution avec pipes (|) : 
           - Utilisation de pipe() pour relier plusieurs commandes.
           - Gestion des entrées/sorties entre processus.

4. Gestion des erreurs :
            - Affichage d'un message en cas de commande introuvable ou problème de permission.

5. Exécution en arrière-plan (&) :

   	    - Détection du & et suppression de wait() pour éviter l’attente.
            - Gestion des processus en arrière-plan via SIGCHLD.

6. Gestion des zombies :
            - Utilisation d’un gestionnaire de signal SIGCHLD avec waitpid() pour nettoyer les processus terminés.

7. Gestion des jobs :
	- Stockage des jobs dans une table des jobs avec : Numéro, PID, état et commande.
	- Commande jobs pour afficher les jobs en arrière-plan.
	- Commandes pour manipuler les jobs :
	- fg <num_job> : Ramène un job au premier plan.
 	- bg <num_job> : Reprend un job en arrière-plan.
	- stop <num_job> : Suspend un job.

8. Gestion des signaux (Ctrl-C, Ctrl-Z) :
	- Bloque l’interruption du shell principal.
	- Redirige SIGINT et SIGTSTP vers les processus en cours.

Conclusion: 

Ce mini-shell reprend les principales fonctionnalités d’un shell Unix, avec la gestion des commandes, des redirections, des pipes, des processus en arrière-plan et des signaux. Il constitue une base solide pour l’apprentissage des concepts de gestion des processus et des entrées/sorties sous Unix.

