#ifndef EXECUTION_CMD_H
#define EXECUTION_CMD_H

#include <stdio.h>
#include <stdlib.h>
#include "readcmd.h"
#include "csapp.h"
#include <unistd.h>
#include <string.h>   

#include "tube.h"
#include "gestion_job.h"

/*Entrée : Un objet cmdline* l contenant les arguments de la commande et un entier i qui est l'index de la commande dans la séquence.
Sortie : Aucun retour, mais un message d'erreur est affiché si la commande échoue.
But : Change le répertoire de travail courant à celui spécifié par l'argument de la commande cd. Si aucun argument n'est fourni, un message d'erreur est affiché.*/
void cmd_cd(struct cmdline* l, int i);



/*Entrée : Aucun argument.
Sortie : Aucun retour, mais affiche le répertoire courant sur la sortie standard.
But : Affiche le répertoire courant du shell.*/
void cmd_pwd();



/*Entrée : Aucun argument.
Sortie : Aucun retour, termine le programme avec un code de sortie 0.
But : Quitte le programme.*/
void cmd_exit();



/*Entrée : Un objet cmdline* l contenant les arguments de la commande et un entier i qui est l'index de la commande dans la séquence.
Sortie : Aucun retour, mais affiche les arguments de la commande echo sur la sortie standard.
But : Affiche les arguments passés à la commande echo.*/
void cmd_echo(struct cmdline* l, int i);



/*Entrée : Un objet cmdline* l contenant les arguments de la commande et un entier i qui est l'index de la commande dans la séquence.
Sortie : Aucun retour, mais un message d'erreur est affiché si la commande échoue.
But : Définit une variable d'environnement avec la valeur passée en argument.*/
void cmd_export(struct cmdline* l, int i);



/*Entrée : Un objet cmdline* l contenant les arguments de la commande et un entier i qui est l'index de la commande dans la séquence.
Sortie : Aucun retour, mais un message d'erreur est affiché si la commande échoue.
But : Envoie le signal SIGTERM pour tuer un processus identifié par son PID.*/
void cmd_kill(struct cmdline* l, int i);



/*Entrée : Aucun argument.
Sortie : Aucun retour, mais affiche l'état des jobs en cours.
But : Affiche les jobs en cours avec leur numéro et leur état.*/
void cmd_jobs();



/*Entrée : Un entier numero_job représentant le numéro du job à mettre en arrière-plan.
Sortie : Aucun retour.
But : Met le job spécifié en arrière-plan.*/
void cmd_bg(int numero_job);



/*Entrée : Un objet cmdline* l contenant la séquence de commandes et un entier i représentant l'index de la commande à exécuter.
Sortie : Aucun retour.
But : Exécute la commande interne correspondante (par exemple cd, pwd, exit, etc.) en fonction du contenu de l->seq[i].*/
void executer_commande_interne(struct cmdline* l , int i);

#endif