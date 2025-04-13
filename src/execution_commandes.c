#include "execution_commandes.h"

/******************************************************  Commande internes ************************************************************************/
/**************************************************************************************************************************************************/

// Fonction pour la commande "cd" (changer de répertoire)
void cmd_cd(struct cmdline* l, int i) {
    // Vérifier si un argument est donné après cd (nom du répertoire)
    if (l->seq[i][1] != NULL) { 
        // Tenter de changer de répertoire
        if (chdir(l->seq[i][1]) == -1) {
            // Si la commande échoue, afficher un message d'erreur
            perror("cd a échoué");
        }
    } else {
        // Si aucun argument n'est passé à la commande, afficher une erreur
        fprintf(stderr, "cd : argument manquant\n");
    }
}

/*********************************************************/

// Fonction pour la commande "pwd" (afficher le répertoire courant)
void cmd_pwd() {
    char cwd[1024];
    // Récupérer le répertoire courant
    if (getcwd(cwd, sizeof(cwd)) != NULL) {
        // Si réussi, afficher le répertoire courant
        printf("%s\n", cwd);
    } else {
        // Si l'appel échoue, afficher un message d'erreur
        perror("pwd a échoué");
    }
}

/**********************************************************/

// Fonction pour la commande "exit" (terminer le programme)
void cmd_exit() {
    // Quitter le programme proprement 
    exit(0);
}

/*********************************************************/


// Fonction pour la commande "echo" (afficher les arguments)
void cmd_echo(struct cmdline* l, int i) {
    int j = 1;  // Commencer à 1 pour ignorer le nom de la commande
    // Parcourir les arguments après "echo"
    while (l->seq[i][j] != NULL) {
        // Afficher chaque argument
        printf("%s ", l->seq[i][j]);
        j++;
    }
    // Ajouter une nouvelle ligne à la fin
    printf("\n");
}


/********************************************************/


// Fonction pour la commande "export" 
void cmd_export(struct cmdline* l, int i) {
    // Vérifier si un argument est donné après export 
    if (l->seq[i][1] != NULL) {  
        // Tenter d'ajouter ou de modifier une variable d'environnement
        if (putenv(l->seq[i][1]) != 0) {
            // Si l'ajout échoue, afficher un message d'erreur
            perror("export a échoué");
        }
    } else {
        fprintf(stderr, "export : argument manquant\n");
    }
}

/********************************************************/


// Fonction pour la commande "kill" (terminer un processus)
void cmd_kill(struct cmdline* l, int i) {
    // Vérifier si un argument est donné après kill 
    if (l->seq[i][1] != NULL) {  
        // Convertir l'argument en un ID de processus 
        pid_t pid = atoi(l->seq[i][1]);
        // Envoyer le signal SIGTERM pour terminer le processus
        if (kill(pid, SIGTERM) == -1) {
            // Si l'envoi du signal échoue, afficher un message d'erreur
            perror("kill a échoué");
        }
    } else {
        // Si aucun argument n'est passé à la commande, afficher une erreur
        fprintf(stderr, "kill : argument manquant\n");
    }
}

/*******************************************************/


// Fonction pour la commande "jobs" (afficher les jobs en cours)
void cmd_jobs(){
    // Parcourir tous les jobs enregistrés
    for (int i = 0; i < MAXJOBS; i++) {
        // Si le job est en cours ou en arrière-plan, l'afficher
        if(jobs[i].etat == 1 || jobs[i].etat == 3){
            printf(" [%d] %d    %s      %s \n", jobs[i].numero_job , jobs[i].pid , etat_job_str[jobs[i].etat],  jobs[i].ligne_commande);
        }
    }
}


/*****************************************************/

// Fonction pour exécuter une commande interne selon le type de commande
void executer_commande_interne(struct cmdline* l , int i) {
  
    // Vérifier le type de commande à appliquer
    if (l->seq[i][0] != NULL) {
        if (strcmp(l->seq[i][0], "cd") == 0) {
            cmd_cd(l, i);
        } else if (strcmp(l->seq[i][0], "pwd") == 0) {
            cmd_pwd();
        } else if (strcmp(l->seq[i][0], "logout") == 0) {
            cmd_exit();
        } else if (strcmp(l->seq[i][0], "echo") == 0) {
            cmd_echo(l, i);
        } else if (strcmp(l->seq[i][0], "export") == 0) {
            cmd_export(l, i);
        } else if (strcmp(l->seq[i][0], "kill") == 0) {
            cmd_kill(l, i);
        } else if (strcmp(l->seq[i][0], "jobs") == 0){
            cmd_jobs();
        }else if (strcmp(l->seq[i][0], "fg") == 0){
            cmd_fg(atoi(l->seq[i][1]));
        }else if (strcmp(l->seq[i][0], "bg") == 0){
            cmd_bg(atoi(l->seq[i][1]));
        }else if (strcmp(l->seq[i][0], "stop") == 0){
            cmd_stp(atoi(l->seq[i][1]));
        }else {
            fprintf(stderr, "Commande non reconnue\n");
        }
    }
}
