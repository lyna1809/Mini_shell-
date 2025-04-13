/*
 * Copyright (C) 2002, Simon Nieuviarts
 */
# include "shell.h"
 
// Définition de commandes_internes : 
const char *commandes_internes[] = {
    "cd", "pwd", "exit", "logout", "help", "echo",
    "export", "unset", "set", "declare", "local",
    "jobs", "fg", "bg", "kill", "wait", "disown",
    "umask", "ulimit", "hash", "type",
    "alias", "unalias", "command", "builtin",
    "source", "eval", "history","stop",
    NULL 
};


/****************************************************************/
/****************************************************************/


// Fonction permettant de vérifier si une commande est une commande interne
int est_commande_interne(const char *cmd) {
    for (int i = 0; commandes_internes[i] != NULL; i++) {
        if (strcmp(cmd, commandes_internes[i]) == 0) {
            return 1; 
        }
    }
    return 0; 
}

/**************************************************************/
/**************************************************************/

// Fonction principale d'exécution des commandes
void executer_cmd(struct cmdline* l) {
    int copie_stdin = -1, copie_stdout = -1; 

    // Sauvegarde des descripteurs de fichier standard
    copie_stdin = dup(STDIN_FILENO); 
    if (copie_stdin == -1) {
        perror("Erreur lors de la copie de stdin\n");
        return;
    }

    copie_stdout = dup(STDOUT_FILENO); 
    if (copie_stdout == -1) {
        perror("Erreur lors de la copie de stdout\n");
        return;
    }

    // Vérification si plusieurs commandes sont présentes 
    if (nb_commandes(l) > 1 && gestion_tube(l, &copie_stdin, &copie_stdout) == -1) {
        printf("Erreur lors de la gestion des tubes\n");
        return;

    } else if (nb_commandes(l) == 1) { // Si une seule commande
        // Gestion des redirections d'entrée et sortie
        if (l->out != NULL && redirection_sortie(l->out, &copie_stdout) == -1) return; 
        if (l->in != NULL && redirection_entree(l->in, &copie_stdin) == -1) return;

        char *cmd = l->seq[0][0]; // Récupération de la commande
        if (est_commande_interne(cmd)) {
            executer_commande_interne(l, 0); // Exécution d'une commande interne
        } else {
            executer_commande_externe(l, 0); // Exécution d'une commande externe
        }
    }

    // Rétablissement des descripteurs d'entrée et de sortie
    if (copie_stdin != -1) {
        dup2(copie_stdin, STDIN_FILENO);
        close(copie_stdin);
    }
    if (copie_stdout != -1) {
        dup2(copie_stdout, STDOUT_FILENO);
        close(copie_stdout);
    }
}



/****************************************************************************/
/****************************************************************************/


int main(){
	 
	// Gestion des signaux pour éviter que des processus fils deviennent zombies
    Signal(SIGCHLD, handler_sigchild);
    Signal(SIGINT,  handler_sigint);  // Gestion du signal d'interruption (Ctrl+C)
    Signal(SIGTSTP, handler_sigtstp); // Gestion du signal de suspension (Ctrl+Z)

    while (1) {
        struct cmdline *l;
        
        
        printf("shell> ");
        l = readcmd(); 
   
        if (!l) {
            printf("exit\n");
            exit(0);
        }

        // Gestion des erreurs de syntaxe
        if (l->err) {
            printf("error: %s\n", l->err);
            continue;
        }

        // Ignorer les commandes vides
        if (l->seq[0] == NULL) {
            continue;
        }

        // Vérification des commandes de fin (exit, quit)
        if (strcmp(l->seq[0][0], "exit") == 0 || strcmp(l->seq[0][0], "quit") == 0) {
            exit(0);
        } 

        // Exécution de la commande
        executer_cmd(l);
    } 
}