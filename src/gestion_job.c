#include "gestion_job.h"


// Tableau des états de job sous forme de chaînes de caractères
const char *etat_job_str[] = {
    "LIBRE",           // Etat 0 
    "EN EXECUTION",    // Etat 1 
    "PREMIER PLAN",    // Etat 2 
    "SUSPENDU"         // Etat 3 
};  


/************************************************************************************/
/************************************************************************************/


// Initialise le tableau des jobs en définissant toutes les cases comme libres.
void initialiser_jobs() {
    for (int i = 0; i < MAXJOBS; i++) {
        jobs[i].numero_job = 0;                  
        jobs[i].pid = 0;                         
        jobs[i].etat = JOB_LIBRE;                
        jobs[i].ligne_commande[0] = '\0';  
        jobs[i].nb_processus = 0 ;      
    }
}

/***************************************************************************************/
/***************************************************************************************/


// Recherche l'indice de la première case libre dans le tableau des jobs.
// Retourne l'indice de la première case libre, ou -1 si le tableau est plein.
int premier_indice_libre() {
    for (int i = 0; i < MAXJOBS; i++) {
        if (jobs[i].etat == JOB_LIBRE) {
            return i;  // Trouvé une case libre
        }
    }
    return -1;  // Aucune case libre disponible
}

/***************************************************************************************/
/***************************************************************************************/


// Recherche le numéro du job en premier plan .
// Retourne le numéro du job en premier plan, ou -1 s'il n'y en a pas.
int job_premier_plan() {
    for (int i = 0; i < MAXJOBS; i++) {
        if (jobs[i].etat == JOB_PREMIER_PLAN) {
            return jobs[i].numero_job;  // Retourne le numéro du job en premier plan
        }
    }
    return -1;  // Aucun job en premier plan
}


/*********************************************************************************/
/*********************************************************************************/


// Ajoute un nouveau job dans le tableau des jobs.
// Si le tableau est plein, retourne -1.
// Si l'ajout réussit, retourne 0.
int ajouter_job(pid_t pid, etat_job etat, const char * commande) {
    // Cherche un indice libre pour insérer le nouveau job
    int indice = premier_indice_libre();

    if (indice == -1) {  // Si le tableau est plein
        return -1;  
    }

    // Remplir la structure du job à l'indice trouvé
    jobs[indice].numero_job = indice + 1;  
    jobs[indice].pid = pid;               
    jobs[indice].etat = etat;              
    strncpy(jobs[indice].ligne_commande, commande, sizeof(jobs[indice].ligne_commande) - 1);  
    jobs[indice].ligne_commande[sizeof(jobs[indice].ligne_commande) - 1] = '\0';  
    return 0;
} 


/******************************************************************************/
/*****************************************************************************/


// Supprime un job du tableau des jobs en mettant sa case comme libre.
// Retourne 0 en cas de succès, -1 si le job n'est pas trouvé.
int supprimer_job(int numero_job) {
    for (int i = 0; i < MAXJOBS; i++) {
        if (jobs[i].numero_job == numero_job) {  // Si le job avec ce numéro existe
            jobs[i].numero_job = 0;              
            jobs[i].pid = 0;                      
            jobs[i].etat = JOB_LIBRE;             
            jobs[i].ligne_commande[0] = '\0'; 
            jobs[i].nb_processus = 0 ;       
            return 0;  
        }
    }
    return -1;  // Job non trouvé
}


/****************************************************************************/
/****************************************************************************/



// Met à jour l'état d'un job en fonction de son PID.
// Affiche également l'état du job mis à jour.
void mettre_a_jour_statut_job(pid_t pid, etat_job nouvel_etat) {
    for (int i = 0; i < MAXJOBS; i++) {
        if (jobs[i].pid == pid) {  // Si le PID correspond à un job existant
            jobs[i].etat = nouvel_etat;  // Met à jour l'état du job
            printf(" [%d]+ %s     %s \n", jobs[i].numero_job, etat_job_str[jobs[i].etat], jobs[i].ligne_commande);
            return;  
        }
    }
}

/*************************************************************************/
/*************************************************************************/



// Fonction pour obtenir le numéro de job à partir du PID.
// Retourne le numéro du job correspondant, ou -1 si le job n'est pas trouvé.
int obtenir_numero_job_par_pid(pid_t pid) {
    for (int i = 0; i < MAXJOBS; i++) {
        if (jobs[i].pid == pid) {  // Si le PID correspond à un job
            return jobs[i].numero_job;  // Retourne le numéro du job
        }
    }
    return -1; 
}


/************************************************************************/
/************************************************************************/



// Fonction pour afficher les informations de tous les jobs dans le tableau.
void afficher_jobs() {
    printf("Contenu des jobs :\n");
    for (int i = 0; i < MAXJOBS; i++) {
        if (jobs[i].etat != JOB_LIBRE) {
            printf("Job %d :\n", jobs[i].numero_job); 
            printf("  PID : %d\n", jobs[i].pid);  
            printf("  État : %d\n", jobs[i].etat); 
             printf("  Commande : %s\n", jobs[i].ligne_commande);  
        }
    }
    printf("\n");
}