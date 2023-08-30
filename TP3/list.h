#include <dirent.h>
#include <sys/syscall.h>     
#include <unistd.h>
#include <stdio.h>
#include <sys/stat.h> //for infos
#include <time.h> //pour imprimer le temps
#include <string.h>
#include <fcntl.h>
#include <libgen.h>//pour basename
//manque les mêmes permissions, -f pour les liens en tant que lien (06.07)
//copie dossier, mais s'arrête de copier les fichiers d'éxtérieurs

//Variable globale ==> parce qu'il sont utilitsée presque partout dans chaque fonction
//best practice c'est de les garder locales et les passer à chaque fois
int minmimal_list = 0, compteur_boucle_copy = 0;
struct stat infos;
struct dirent *read_file_directory_dirent;
struct stat infos_entree, infos_destination;
struct dirent *read_entry_file_directory_dirent, *read_destination_file_directory_dirent;

//sous fonctions
DIR* ouvre_dossier(char * chemin);
void print_permissions(mode_t st_mode, mode_t table_de_mode_a_verifier[], char* lettre_mode_a_afficher[]);

void print_time(struct stat infos);
    /* details que j'ai utiliser pour le format de strftime , le reste clair: Selon MAN7:
        %a     abbriviated day

        %b     The abbreviated month name according to the current
              locale

        %d     The day of the month as a decimal number (range 01 to 31).
        */

void open_and_list_dossier( char* chemin);


void copy_file_to_destination_folder(char* entree, char* destination_path);

void  copy(char *entree, char *destination_path);
    // Étape 0 : ouverture du fichier/dossier d'entrée, ouverture du fichier/dossier de destination
    // Gestion des cas nuls
    //Lacks: same permissions
    //Lecture des fichiers de manière récursive
    //la destination dans ce cas est un dossier
int main(int argc,char ** argv);
