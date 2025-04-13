#ifndef __ENTREE_SORTIE_H
#define __ENTREE_SORTIE_H

#include <stdio.h>
#include <stdlib.h>
#include "readcmd.h"
#include "csapp.h"   
#include <unistd.h>

#include "execution_commandes.h" 

/*Entrée : Un nom de fichier (char* nom_fichier), un pointeur vers l'ancien descripteur de fichier stdin (int* copie_stdin).
Sortie : Retourne 0 en cas de succès, -1 en cas d'erreur.
But : Redirige l'entrée standard (stdin) vers le fichier spécifié.*/
int redirection_entree( char* nom_fichier , int* copie_stdin );


/*Entrée : Un nom de fichier (char* nom_fichier), un pointeur vers l'ancien descripteur de fichier stdout (int* copie_stdout).
Sortie : Retourne 0 en cas de succès, -1 en cas d'erreur.
But : Redirige la sortie standard (stdout) vers le fichier spécifié.*/
int redirection_sortie( char* nom_fichier , int* copie_stdout);



#endif