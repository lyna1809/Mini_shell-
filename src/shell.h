#ifndef __SHELL_H__
#define __SHELL_H__

#include <stdio.h>
#include <stdlib.h>
#include "readcmd.h"
#include "csapp.h"
#include <unistd.h>

#include "tube.h"
#include "gestion_job.h"


extern const char *commandes_internes[]; 


/* Entrée : La commande sous forme de chaîne de caractères.
   sortie : 1 si la commande est interne, 0 sinon.
but : Cette fonction compare la commande donnée avec la liste des 
commandes internes prédéfinies et retourne 1 si elle en fait partie, 
sinon elle retourne 0.*/
int est_commande_interne(const char *cmd) ;



/* Entrée : Pointeur vers une structure cmdline contenant 
        la séquence de commandes et leurs arguments.
Sotie : Rien .
But : Cette fonction prend en charge l'exécution d'une commande 
    en effectuant les étapes suivantes :
    Sauvegarde des descripteurs de fichier standard (stdin, stdout).
    Vérification de la présence de pipes et gestion des tubes si nécessaire.
    Gestion des redirections d'entrée et de sortie.
    Vérification du type de commande (interne ou externe) et exécution 
    appropriée.
    Restauration des descripteurs de fichier après l'exécution.*/
void executer_cmd(struct cmdline* l) ;


int main(); 


#endif 