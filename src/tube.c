#include "tube.h"

//declaration d'une variable globale pour stocker les processus en fg 
fg_processus pids_table = { .nb = 0 };

//Var pour PGID des processus en fg 
volatile sig_atomic_t pgid_premier_plan = 0;


/*############################################LES HANDLER ######################################################################*/
/*##############################################################################################################################*/

//gestion du cntrl-c
void handler_sigint(int sig) {
    if (pgid_premier_plan != 0) {
        //envoie SIGINT a tout le groupe de processus de fg
        kill(-pgid_premier_plan, SIGINT);
    }
}

/*####################################################*/

//gestion du cntrl-z
void handler_sigtstp(int sig) { 
    if (pgid_premier_plan != 0) {
        //envoie SIGTSTP au groupe de processus de fg
        for (int i = 0; i < MAXJOBS; i++) {
            if (jobs[i].pid == pgid_premier_plan) {
                jobs[i].nb_processus= pids_table.nb;
                copy_pid_table(pids_table.pids, jobs[i].pids , pids_table.nb);
            }
        }
        kill(-pgid_premier_plan, SIGTSTP);  
    }
}

/*####################################################*/

//gestion des zombies 
void handler_sigchild(int sig) { 
    pid_t pid;
    int status;
    
    while ((pid = waitpid(-1, &status, WNOHANG | WUNTRACED)) > 0) {
        
        //verifier si le processus en premier plan a ete suspendu 
        if (WIFSTOPPED(status)) {
            mettre_a_jour_statut_job(pid, JOB_SUSPENDU);
        }
        //si le processus s'est termine ou tué
        else if (WIFEXITED(status) || WIFSIGNALED(status)) {
            int num_job= obtenir_numero_job_par_pid(pid);
            supprimer_job(num_job);
        }
        remove_fg_process(pid);
    }
    if (pid == -1 && errno != ECHILD)
        unix_error("waitpid error");
    return;
}

/*############################## Fonctions pour gestion de la structure des processus en fg #####################################*/
/*##############################################################################################################################*/

//fonction pour supprimer un PID du tableau 
void remove_fg_process(pid_t pid) {
    for (int i = 0; i < pids_table.nb; i++) {
        if (pids_table.pids[i] == pid) {
            //decalage des éléments pour combler le trou
            for (int j = i; j < pids_table.nb - 1; j++) {
                pids_table.pids[j] = pids_table.pids[j + 1];
            }
            pids_table.nb--; 
            break;
        }
    }
}

/************************************************************* */

// Copier table 1 a 2 
int copy_pid_table(pid_t table1[], pid_t table2[], int nb_pids) {
    if (nb_pids > Max_fg_process) {
        return -1;
    }
    for (int i = 0; i < nb_pids; i++) {
        table2[i] = table1[i];
    }
    return nb_pids;
}

/*********************************************************** */

void add_fg_process(pid_t pid) {
    if (pids_table.nb < Max_fg_process) {
        pids_table.pids[pids_table.nb] = pid;
        pids_table.nb++;
    } else {
        fprintf(stderr, "Erreur : trop de processus en fg \n");
    }
}

/************************************************************ */

int fg_est_vide(){
    if (pids_table.nb ==0){
        return 1;  //si y a plus d'elemnt dans la structure 
    }
    return 0 ; 
}


/*############################################ Gestion des tubes #####################################################################*/
/*################################################################################################################################*/

int nb_commandes (struct cmdline* l){
    int size=0; 
    while(l->seq[size] != 0){
        size++;
    }
    return size;
}

/********************************************************* */

void free_tubes(int* tubes[], int nb_cmd) {
    for (int i = 0; i < nb_cmd - 1; i++) {
        free(tubes[i]);  
    }
}

/******************************************************** */


void attente_active(){
    while (fg_est_vide()!= 1){
        sleep(1); 
    }
}

/***************************************************** */
int creation_tubes(int nb_cmd , int **tubes){
    for (int i = 0; i < nb_cmd - 1; i++) {
        tubes[i] = (int*)malloc(2 * sizeof(int));  
        if (tubes[i] == NULL) {
            perror("Erreur d'allocation mémoire pour le pipe");
            return -1;
        }
        if (pipe(tubes[i]) == -1) {
            perror("Erreur lors de la création du pipe");
            return -1;
        }
    }
    return 0 ;
}

/******************************************************* */

void Gestion_premiere_commande(int **tubes , int nb_cmd ){
    int i =0 ;
    int* mypipe= tubes[i];
    close(mypipe[0]);
    for(int j= i+1; j < nb_cmd -1; j++){ //fermeture de tous les autres pipes 
        int* mypipe_k= tubes[j];
        close(mypipe_k[0]);
        close(mypipe_k[1]);
    }

    // Rediriger sa sortie (stdout) vers le pipe 
    if (dup2(mypipe[1], STDOUT_FILENO) == -1) {
        perror("Erreur lors de la redirection de sortie vers le tube 1\n");
        exit(1);  
    }
    close(mypipe[1]); // Fermer après duplication             
}

/*************************************************************************** */

void Gestion_derniere_commande(int **tubes , int nb_cmd ){
    int i = nb_cmd-1 ; 
    int* mypipe= tubes[i-1]; 
    close(mypipe[1]);

    for(int j= 0; j< nb_cmd- 2; j++){ //fermeture de tous les autres pipes 
        int* mypipe_k= tubes[j];
        close(mypipe_k[0]);
        close(mypipe_k[1]);
    }

    // Rediriger son entree(stdin) vers le pipe 
    if (dup2(mypipe[0], STDIN_FILENO) == -1) {
        perror("Erreur lors de la redirection de l'entree  vers le tube\n");
        exit(1);  
    }
    close(mypipe[0]); // Fermer après duplication
}

/****************************************************************************** */

void redirection_entree_sortie_pipe (int* mypipe1,int* mypipe2){
    // Rediriger sa sortie (stdout) vers le pipe 
    if (dup2(mypipe2[1], STDOUT_FILENO) == -1) {
        perror("Erreur lors de la redirection de sortie vers le tube   \n");
        exit(1);  
    }

    // Rediriger son entree(stdin) vers le pipe 
    if (dup2(mypipe1[0], STDIN_FILENO) == -1) {
        perror("Erreur lors de la redirection de l'entree vers le tube\n");
        exit(1);  
    }
}

/******************************************************************************** */

int gestion_tube(struct cmdline* l ,int* copie_stdin,  int* copie_stdout ){
    int nb_cmd= nb_commandes(l);
    int* tubes [nb_cmd];

    //var pour stocker pgid des cmd des tubes 
    pid_t pgid_cmds = 0;

    if(creation_tubes(nb_cmd , tubes) == -1) { printf("erreur lors de la creation des tubes \n");
    return -1 ; }

    // creation des fils 
    for (int i=0; i< nb_cmd; i++) {

        //creation des fils 
        pid_t pid = fork();
        if (pid < 0) {
            unix_error("Erreur lors de la création du processus fils : fork()\n");
            return -1;
        }
        
        if(pid == 0){ // fils
            // Affecter le processus au groupe de la pipeline
            if (i == 0) { 
                // Le premier processus devient le leader du groupe
                if (setpgid(0, 0) < 0) {
                    perror("erreur setpgid pour le premier processus \n");
                    exit(1);
                }
                pgid_cmds = getpid(); // Le PGID du groupe devient le PID du leader

            } else {
                // Pour les autres, se placer dans le groupe du premier processus
                if (setpgid(0, pgid_cmds) < 0) {
                    perror("erreur setpgid pour un processus intermediaire  \n");
                    exit(1);
                }
            }

            if(i==0){ // la premiere cmd 
                Gestion_premiere_commande(tubes , nb_cmd ) ; 
                //si y'a une rederection d'entreee externe : < 
		        if (l->in != NULL && redirection_entree(l->in , copie_stdin )== -1) return -1;
            }

            else if ( i == nb_cmd-1){ // la derniere cmd 
                Gestion_derniere_commande(tubes ,nb_cmd ); 
                //si y'a une rederection de sortie externe 
                if (l->out != NULL && redirection_sortie(l->out , copie_stdout )==-1) return -1;
            }
            else{ // cmd intermed

                int* mypipe1= tubes[i-1];
                int* mypipe2= tubes[i];

                close(mypipe1[1]);
                close(mypipe2[0]);

                // Fermeture des autres pipes  
                for(int j=0; j < nb_cmd -1; j++) {  
                    if (j != i-1 && j != i) { // Ne ferme pas ceux qu'on utilise encore
                        close(tubes[j][0]);  
                        close(tubes[j][1]);  
                    }  
                }
                redirection_entree_sortie_pipe (mypipe1,mypipe2) ; 

                close(mypipe2[1]); // Fermer après duplication
                close(mypipe1[0]); // Fermer après duplication
            }
            execvp(l->seq[i][0], l->seq[i]);
            if (errno == ENOENT) {
                fprintf(stderr, "Erreur : commande introuvable : %s\n", l->seq[i][0]);
                exit(1);
            } else {
                unix_error("Erreur d'exécution : execvp()\n");
                exit(1);
            }

        }
        
        else { //parent
            // Dans le parent, assurer que chaque enfant est bien placé dans le groupe
            if (i == 0) {
                pgid_cmds = pid;  // Pour le premier enfant, son PID devient le PGID de la pipeline
                if (setpgid(pid, pid) < 0) {
                    perror("Erreur setpgid dans le parent pour le premier enfant");
                }
            } else {
                if (setpgid(pid, pgid_cmds) < 0) {
                    perror("Erreur setpgid dans le parent pour un enfant");
                }
            }
            // Ajout du job dans la table des jobs (on ajoute une seule entrée pour la pipeline)
            if (i == 0) {
                if (l->is_in_bg == 0) {
                    // Job en premier plan
                    ajouter_job(pgid_cmds, JOB_PREMIER_PLAN, l->seq[0][0]);
                    pgid_premier_plan = pgid_cmds;  // Stocker le PGID global pour les handlers
                } else {
                    // Job en arrière-plan
                    ajouter_job(pgid_cmds, JOB_EN_EXECUTION, l->seq[0][0]);
                }
            }

            if(l->is_in_bg ==0){
                add_fg_process(pid);
            }
            
        }
    }
    // Close all pipes in parent
    for (int i = 0; i < nb_cmd - 1; i++) {
        close(tubes[i][0]);
        close(tubes[i][1]);
    }
    // Attendre la mort des fils 
    /*  for(int i=0 ; i< nb_cmd ; i++){
        //printf("je suis dans la boucle  \n");
        int status; 
        //en fg
        waitpid(-1, &status, 0);  
    }*/
    
    //attente active 
    if (l->is_in_bg != 1){
        attente_active();
        pgid_premier_plan=0 ; 
    }
  
    free_tubes(tubes, nb_cmd);
    return 0;
}

/*############################################ Gestion commandes externes ######################################################################*/
/*#################################################################################################################################*/


void executer_commande_externe(struct cmdline* l,int i ){ 
	//cree un processur fils , pour qu'il s'occupe d'executer la commande 
	pid_t pid = fork();
    pid_t pgid_cmds = pid ;
    

	if (pid <0 ){
		unix_error("Erreur fork");
	}

	if (pid ==0 ){ //fils
        // Le premier processus devient le leader du groupe
        if (setpgid(0, 0) < 0) {
            perror("erreur setpgid pour le premier processus \n");
            exit(1);
        }
        pgid_cmds = getpid(); // Le PGID du groupe devient le PID du leader
        
		//processus fils : il va executer la commande 
		//si le commande s'execute correctement le processur fils sera meurt 
		execvp(l->seq[i][0], l->seq[i]); 

		//sinon (un probleme lors de execvp)
		//on détecte si la commande n'existe pas 
		if (errno == ENOENT) {
            fprintf(stderr, "Erreur : commande introuvable : %s\n", l->seq[i][0]);
            exit(127);  
        } else {
            unix_error("Erreur execvp");
		}

	} else { //parent

        pgid_cmds = pid;  // Pour le premier enfant, son PID devient le PGID de la pipeline
        if (setpgid(pid, pid) < 0) {
            perror("Erreur setpgid dans le parent pour le premier enfant");
        }

        if (l->is_in_bg == 0) {
            add_fg_process(pid);
            // Job en premier plan
            ajouter_job(pgid_cmds, JOB_PREMIER_PLAN, l->seq[0][0]);
            pgid_premier_plan = pgid_cmds;  // Stocker le PGID global pour les handlers
        } else {
            // Job en arrière-plan
            ajouter_job(pgid_cmds, JOB_EN_EXECUTION, l->seq[0][0]);
        }

        if(l->is_in_bg){   
            //n'attend pas ses fils
        } 
        else{ //en fg
            if (l->is_in_bg != 1){ // attente des fils en fg 
                attente_active();
                pgid_premier_plan=0 ; 
            } 
        }
	}
}

/******************************************FG _ BG _ STOP ************************************************************************************* */
/*********************************************************************************************************************************************** */

void cmd_fg( int numero_job ){ // de bg a fg ou de sespendu a fg 
    pid_t pid;

    for (int i = 0; i < MAXJOBS; i++) {
      if(jobs[i].numero_job == numero_job){

        pid = jobs[i].pid; // pid est pgid de group 

         // si suspendu remettre au travail  : SIGCONT
         if(jobs[i].etat == JOB_SUSPENDU ){

            printf("cmd : %s \n", jobs[i].ligne_commande);

            if(kill(-pid, SIGCONT) < 0){
                perror("SIGCONT() : Erreur lors le repend de travail\n");
                return ;
            } 

            for(int j=0 ; j <jobs[i].nb_processus ; j++){
                add_fg_process(jobs[j].pids[j]);
            }    

            jobs[i].etat = JOB_PREMIER_PLAN; //modification de status au fg 
            pgid_premier_plan= pid;  // mettre le pgid de group en premier plan 
            
        } else if (jobs[i].etat == JOB_EN_EXECUTION ){
            printf("cmd : %s \n", jobs[i].ligne_commande);

            for(int j=0 ; j <jobs[i].nb_processus ; j++){
                add_fg_process(jobs[j].pids[j]);
            }    

            jobs[i].etat = JOB_PREMIER_PLAN; //modification de status au fg 
            pgid_premier_plan= pid;  // mettre le pgid de group en premier plan 
        }

            // pour que le parent va attendre 
            attente_active();
            pgid_premier_plan=0 ;
            return ; 
      }
    }

    printf("[%d] non trouve \n", numero_job);
    

}

/*************************************************************************************** */

void cmd_bg(int numero_job) {
    for (int i = 0; i < MAXJOBS; i++) {
        if (jobs[i].numero_job == numero_job) {
           // si suspendu continue 
            if (jobs[i].etat == JOB_SUSPENDU) {
                pid_t pgid = jobs[i].pid;

                if (kill(-pgid, SIGCONT) < 0) { 
                    perror("SIGCONT() : Erreur lors le repend de travail\n");
                    return;
                }
                printf("[%d] resume en arrier plan : %s\n", numero_job, jobs[i].ligne_commande);

                // changer le status 
                jobs[i].etat = JOB_EN_EXECUTION;
                return;
            } else {
                return;
            }
        }
    }
    printf(" [%d] non trouve \n", numero_job);
}

/************************************************************************************* */


void cmd_stp(int numero_job) {
    for (int i = 0; i < MAXJOBS; i++) {
        if (jobs[i].numero_job == numero_job) {
            // stop si job est en fg 
            if (jobs[i].etat == JOB_PREMIER_PLAN || jobs[i].etat == JOB_EN_EXECUTION) {
                pid_t pgid = jobs[i].pid;

                if (kill(-pgid, SIGTSTP) < 0) {
                    perror("SIGTSTP() : erreur lors l'arret de job\n");
                    return;
                }
                printf("[%d] arreter %s\n", numero_job, jobs[i].ligne_commande);

                jobs[i].nb_processus = pids_table.nb;
                copy_pid_table(pids_table.pids, jobs[i].pids, pids_table.nb);

                // mise a jour de status 
                jobs[i].etat = JOB_SUSPENDU;
                pgid_premier_plan = 0;  
                return;
            } else {
                return;
            }
        }
    }
    printf("[%d] non trouve \n", numero_job);
}

/****************************************************************************************** */

void affichage (struct cmdline * l){ 
	 
    /* Display each command of the pipe */
    int i, j;
    for (i=0; l->seq[i]!=0; i++) {
        char **cmd = l->seq[i];
        printf("seq[%d]: ", i);
        for (j=0; cmd[j]!=0; j++) {
            printf("%s ", cmd[j]);
        }
        printf("\n");
    }

    if (l->in) printf("in: %s\n", l->in);
    if (l->out) printf("out: %s\n", l->out);	 
}


