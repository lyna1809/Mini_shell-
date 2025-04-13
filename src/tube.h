#ifndef __TUBE_H__
#define __TUBE_H__

#include <stdio.h>
#include <stdlib.h>
#include "readcmd.h"
#include "csapp.h"
#include <unistd.h>

#include "gestion_job.h"
#include "entree_sortie.h"
#include "shell.h"


//Var pour PGID des processus en fg 
extern volatile sig_atomic_t pgid_premier_plan;

//Strcuture pour stoquée les processus qui sont en fg :
typedef struct 
{
    pid_t pids[Max_fg_process]; //Tableau des pids 
    int nb ; //nombre de processus en fg 
} fg_processus;

/******************************************************************************** */

/*Entrée : int sig (le signal SIGINT reçu)
Sortie : Aucune
Effet : Envoie SIGINT à tout le groupe de processus en avant-plan (pgid_premier_plan)*/
void handler_sigint(int sig);

/*Entrée : int sig (le signal SIGTSTP reçu)
Sortie : Aucune
Effet : Enregistre les processus du premier plan dans jobs[] et envoie SIGTSTP au groupe*/
void handler_sigtstp(int sig);

/*Entrée : int sig (le signal SIGCHLD reçu)
Sortie : Aucune
Effet :
    Récupère les processus terminés ou suspendus (waitpid())
    Met à jour jobs[]
    Supprime les processus terminés du tableau des processus en avant-plan (remove_fg_process())*/
void handler_sigchild(int sig) ;


/*Entrée : pid_t pid (PID à supprimer du tableau des processus en avant-plan)
Sortie : Aucune
Effet :
    Supprime pid de pids_table
    Décale les éléments suivants pour combler le trou*/
void remove_fg_process(pid_t pid);



/*Entrée :
    table1[] (tableau source des PIDs)
    table2[] (tableau destination)
    nb_pids (nombre d'éléments à copier)
Sortie : Nombre d'éléments copiés ou -1 si dépassement de Max_fg_process
Effet : Copie table1 dans table2*/
int copy_pid_table(pid_t table1[], pid_t table2[], int nb_pids);



/*Entrée : pid_t pid (PID à ajouter au tableau des processus en avant-plan)
Sortie : Aucune
Effet : Ajoute pid à pids_table si la limite Max_fg_process n'est pas atteinte*/
void add_fg_process(pid_t pid);


/*Entrée : Aucune
Sortie : 1 si pids_table est vide, sinon 0
Effet : Vérifie si des processus sont toujours en avant-plan*/
int fg_est_vide();

/*Entrée : struct cmdline* l (commande analysée)
Sortie : Nombre de commandes dans l->seq
Effet : Calcule le nombre de commandes dans la ligne de commande*/
int nb_commandes(struct cmdline* l);



/*Entrée :
    tubes[] (tableau de pipes)
    nb_cmd (nombre de commandes)
Sortie : Aucune
Effet : Libère la mémoire des tubes*/
void free_tubes(int* tubes[], int nb_cmd);

/*Entrée : Aucune
Sortie : Aucune
Effet : Boucle jusqu'à ce que tous les processus en avant-plan se terminent*/
void attente_active() ;

/*Entrée :

    nb_cmd (nombre de commandes)
    tubes (tableau des tubes)

Sortie : 0 en cas de succès, -1 en cas d’échec
Effet : Alloue et crée des pipes pour la communication entre commandes*/
int creation_tubes(int nb_cmd, int **tubes);


/*Entrée :
    tubes (tableau des tubes)
    nb_cmd (nombre de commandes)
Sortie : Aucune
Effet :
    Ferme les pipes inutiles
    Redirige stdout vers le premier tube*/
void Gestion_premiere_commande(int **tubes , int nb_cmd );

/*Entrée :
    tubes (tableau des tubes)
    nb_cmd (nombre de commandes)
Sortie : Aucune
Effet :
    Ferme les pipes inutiles
    Redirige stdin vers le dernier tube*/
void Gestion_derniere_commande(int **tubes , int nb_cmd );

/*Entrée :
    mypipe1 (pipe d'entrée)
    mypipe2 (pipe de sortie)
Sortie : Aucune
Effet : Redirige stdin et stdout entre deux pipes*/
void redirection_entree_sortie_pipe(int* mypipe1, int* mypipe2);


/*Entrée :
    l (ligne de commande)
    copie_stdin (sauvegarde de stdin)
    copie_stdout (sauvegarde de stdout)
Sortie : 0 en cas de succès, -1 en cas d’échec
Effet :
    Crée et configure des pipes
    Fork les processus
    Gère les redirections d'entrée/sortie*/
int gestion_tube(struct cmdline* l, int* copie_stdin, int* copie_stdout);



/*Entrées :
    struct cmdline* l : Structure contenant la ligne de commande analysée.
    int i : Indice de la commande à exécuter dans l->seq[i].
Sortie :
    Aucune (exécute la commande et ne retourne pas de valeur).
Effet :
    Crée un processus enfant (fork).
    Utilise execvp pour exécuter la commande externe.
    Configure les redirections d'entrée/sortie si nécessaire.
    Attend la fin du processus enfant si la commande est en avant-plan.
    Ajoute le processus aux jobs s'il est en arrière-plan.*/
void executer_commande_externe(struct cmdline* l,int i );


/*Entrées :
    int numero_job : Numéro du job à ramener en avant-plan.
Sortie :
    Aucune (modifie l'état du job mais ne retourne pas de valeur).
Effet :
    Recherche le job correspondant dans la liste des jobs.
    Modifie son état pour le remettre en avant-plan.
    Envoie un signal SIGCONT pour relancer le processus s'il était suspendu.
    Met à jour le groupe de processus du shell pour suivre ce job.
    Attend que le processus se termine avant de redonner la main à l'utilisateur.*/
void cmd_fg( int numero_job );



/*Entrées :
    int numero_job : Numéro du job à exécuter en arrière-plan.
Sortie :
    Aucune.
Effet :
    Recherche le job correspondant dans la liste des jobs.
    Modifie son état pour l'exécuter en arrière-plan.
    Envoie un signal SIGCONT pour reprendre l'exécution s'il était suspendu.
    Ne bloque pas l'exécution du shell (contrairement à cmd_fg).*/
void cmd_bg(int numero_job);



/*Entrées :
    int numero_job : Numéro du job à suspendre.
Sortie :
    Aucune.
Effet :
    Recherche le job correspondant dans la liste des jobs.
    Envoie un signal SIGTSTP pour suspendre le processus.
    Met à jour la liste des jobs pour refléter son état suspendu.*/
void cmd_stp(int numero_job);


#endif