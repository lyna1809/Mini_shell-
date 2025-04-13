#include "entree_sortie.h"


// Fonction qui redirige l'entrée standard (stdin) vers un fichier donné.
int redirection_entree( char* nom_fichier , int* copie_stdin ){

    // Ouvre le fichier en lecture seule (O_RDONLY)
    int fd_in = open( nom_fichier , O_RDONLY);
    if (fd_in == -1) {
        // Si l'ouverture échoue, on affiche une erreur et ferme le fichier de sauvegarde du stdin
        fprintf(stderr, "Erreur lors de l'ouverture du fichier : %s, %s\n", nom_fichier, strerror(errno)); 
        close(*copie_stdin);
        return -1;
    }

    // Redirige l'entrée standard (stdin) vers le fichier ouvert.
    // dup2 remplace le descripteur de fichier STDIN_FILENO (0) par fd_in 
    if (dup2(fd_in, STDIN_FILENO) == -1) {
        // En cas d'échec de la redirection, on ferme le descripteur de fichier et retourne une erreur
        close(fd_in);
        close(*copie_stdin);
        perror("Erreur lors de la redirection de stdin\n");
        return -1;
    }

    // On ferme le descripteur de fichier original car il est maintenant redirigé.
    close(fd_in);

    // La redirection s'est bien passée, on retourne 0.
    return 0;
}


/*##############################################################################################################################################*/



// Fonction qui redirige la sortie standard (stdout) vers un fichier donné.
int redirection_sortie( char* nom_fichier , int* copie_stdout){

    // Ouvre le fichier en mode écriture, avec création et écrasement du contenu existant (O_WRONLY | O_CREAT | O_TRUNC).
    // Les permissions du fichier sont définies comme 0644 (lecture/écriture pour le propriétaire, lecture pour les autres).
    int fd_out = open(nom_fichier , O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (fd_out == -1) {
        // Si l'ouverture échoue, on affiche une erreur et ferme le fichier de sauvegarde du stdout.
        fprintf(stderr, "Erreur lors de l'ouverture du fichier : %s, %s\n", nom_fichier, strerror(errno));
        close(*copie_stdout);
        return -1;
    }

    // Redirige la sortie standard (stdout) vers le fichier ouvert.
    // dup2 remplace le descripteur de fichier STDOUT_FILENO (1) par fd_out (le fichier ouvert).
    if (dup2(fd_out, STDOUT_FILENO) == -1) {
        // En cas d'échec de la redirection, on ferme le descripteur de fichier et retourne une erreur.
        close(fd_out);
        close(*copie_stdout);
        perror("Erreur lors de la redirection de stdout\n");
        return -1;
    }

    // On ferme le descripteur de fichier original car il est maintenant redirigé.
    close(fd_out);

    // La redirection s'est bien passée, on retourne 0.
    return 0;
}