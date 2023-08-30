/*• Deux commandes seront implémentées: 
• exit:le shell se termine proprement
• cd:le shell change le répertoire courant*/

#include <unistd.h>
#include <stdio.h>
#include <errno.h>// pour strerror
#include <limits.h> // pour PATH_MAX
#include <string.h>
#include <unistd.h>
#include <signal.h>


int cd(int argc, char *argv[]) {
    const char *nouveau_chemin_entree=argv[1];
    if (argc != 2) {
        fprintf(stderr, "Usage: cd <directory>\n");
        return -1; 
    }
    if (chdir(nouveau_chemin_entree)< 0) {
            fprintf(stderr, "Error changing directory: %s\n", strerror(errno));
        return -1;
    }
    //affichage cas succés
    char cwd_pour_affichage[256];
    getcwd(cwd_pour_affichage, 256);
    printf("\x1b[32mSuccés\x1b[0m: Nouveau Chemin: %s\n",cwd_pour_affichage );
    return 0; // Return 0 on success
}
