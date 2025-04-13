#ifndef GESTION_JOB_H
#define GESTION_JOB_H

#include <stdio.h>
#include <stdlib.h>
#include "readcmd.h"
#include "csapp.h"
#include <unistd.h> 
#include <string.h>

//taille du tableau des jobs
#define MAXJOBS 10   
#define Max_fg_process 10000


typedef enum {
    JOB_LIBRE,   //case libre  //0
    JOB_EN_EXECUTION, //job en execution   //1 bg 
    JOB_PREMIER_PLAN, //job en execution en premier plan  (fg)  //2
    JOB_SUSPENDU  //3
} etat_job;


//structure pour modeliser les jobs 
typedef struct {
    int numero_job;                    
    pid_t pid;                      
    etat_job etat;                   
    char  ligne_commande[1024];

    int nb_processus; 
    pid_t pids[Max_fg_process];
     
} job_t;


job_t jobs[MAXJOBS];


// Tableau de chaînes de caractères pour représenter les états
extern const char *etat_job_str[] ;

/***************************************************************************************************************/
/****************************************************************************************************************/


/*But : Initialise le tableau des jobs en marquant toutes les entrées comme libres.
Entrée : Aucune.
Sortie : Aucune.
Effet de bord : Modifie le tableau jobs en définissant chaque job avec un numéro 0, un pid à 0, un état JOB_LIBRE et une chaîne de commande vide.*/
void initialiser_jobs();


/*But : Trouver la première case libre dans le tableau des jobs.
Entrée : Aucune.
Sortie : L'indice du premier job libre, ou -1 si le tableau est plein.
Effet de bord : Aucun.*/
int premier_indice_libre();



/*But : Trouver le numéro du job en premier plan.
Entrée : Aucune.
Sortie : Le numéro du job en premier plan s'il existe, sinon -1.
Effet de bord : Aucun.*/
int job_premier_plan();



/*But : Ajouter un nouveau job dans le tableau.
Entrée :
    pid : Identifiant du processus du job.
    etat : État initial du job.
    commande : Chaîne représentant la commande exécutée.

Sortie : 0 en cas de succès.
        -1 si le tableau des jobs est plein.

Effet de bord : Modifie le tableau jobs en ajoutant un nouveau job avec les informations données.*/
int ajouter_job(pid_t pid, etat_job etat, const char *commande);



/*But : Supprimer un job du tableau.
Entrée : numero_job (Numéro du job à supprimer).
Sortie :
    0 si la suppression a réussi.
    -1 si le job n'a pas été trouvé.

Effet de bord : Modifie le tableau jobs en libérant la case correspondante.*/
int supprimer_job(int numero_job);




/*But : Mettre à jour l'état d'un job à partir de son pid.
Entrée :
    pid : Identifiant du processus du job.
    nouvel_etat : Nouvel état à assigner.
Sortie : Aucune.
Effet de bord : Modifie le tableau jobs en mettant à jour l'état du job correspondant au pid.*/
void mettre_a_jour_statut_job(pid_t pid, etat_job nouvel_etat);




/*But : Trouver le numéro du job associé à un pid donné.
Entrée : pid (Identifiant du processus du job).
Sortie :
    Le numéro du job correspondant.
    -1 si aucun job avec ce pid n'existe.
Effet de bord : Aucun.*/
int obtenir_numero_job_par_pid(pid_t pid);



/*But : Afficher la liste des jobs non libres.
Entrée : Aucune.
Sortie : Affiche les informations de chaque job non libre sur la sortie standard.
Effet de bord : Affiche les informations des jobs (numero_job, pid, etat).*/
void afficher_jobs();


#endif 
