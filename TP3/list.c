#include "list.h"
//sous fonctions
DIR* ouvre_dossier(char * chemin){
     if (opendir(chemin)== NULL){
    printf("fonction ouvre_dossier, cas de opendir null, FILE NOT FOUND\n");
    return NULL;}

    DIR *dir = opendir(chemin);
    return dir;
}


void print_permissions(mode_t st_mode, mode_t table_de_mode_a_verifier[], char* lettre_mode_a_afficher[]) {
    for (int mode_number = 0; mode_number < 8; mode_number++) {
        if (st_mode & table_de_mode_a_verifier[mode_number])
            printf("%s", lettre_mode_a_afficher[mode_number]);
        else
            printf("-");
    }
}

void print_time(struct stat infos){
    char buffer_pour_storer_la_chaine_temps[80];
    /* details que j'ai utiliser pour le format de strftime , le reste clair: Selon MAN7:
        %a     abbriviated day

        %b     The abbreviated month name according to the current
              locale

        %d     The day of the month as a decimal number (range 01 to 31).

    */
    strftime(buffer_pour_storer_la_chaine_temps, sizeof(buffer_pour_storer_la_chaine_temps), "%a %b %d %H:%M:%S %Y", localtime(&infos.st_atime));
    printf("     %s", buffer_pour_storer_la_chaine_temps);

}

//Grandes foncions

//void list_dossier(DIR* opened_file_directory, const char * chemin){

//how to printf structures?
//=>rappel sur les pointeurs (5.11)
void open_and_list_dossier( char* chemin){
//returning file list
//opendir ouvre un descripteur de fichier (flux de dossier??) et renvoie le descripteur de fichier fd en return
//on intialise dir par son pointeur, opendir() retourne le pointeur *dir déja
DIR *opened_file_directory = ouvre_dossier(chemin);
if(opened_file_directory == NULL) {printf("chemin dossier incorrect\n");return;}
//Comment lire dire? ==> récursivement?? (30.06)
//==> Boucle tant que on peu lire nextreaddir()
//c'est les fichiers/docs dans le 1er dossier

while ((read_file_directory_dirent = readdir(opened_file_directory)) != NULL) {

    //optionelle
    if (read_file_directory_dirent->d_name[0] == '.') //on va ignorer les fichiers cachés
        continue;
    //


//initialise le string sous_chemin, qu'on va uiliser pour liste les infos de chaque fichier/sous_dossier
char sous_chemin[120];
//on peut utiliser strcat() , mais sprintf + formatting
sprintf(sous_chemin, "%s/%s", chemin, read_file_directory_dirent->d_name);
// stat==> fonction Standard C retourne la structure infos avec les infos du fichier/dossier courant

stat(sous_chemin, &infos );


//check file type
if(S_ISDIR(infos.st_mode)) 
    printf("d");
else     printf("-");

print_permissions(infos.st_mode, (mode_t[]){S_IRUSR, S_IWUSR, S_IXUSR, S_IRGRP, S_IWGRP, S_IXGRP}, (char*[]){"r","w","x","w","r","r","w","x"});
//print file size + file name
printf("  %lld", infos.st_size);

//lld == long long int
print_time(infos);

if (S_ISDIR(infos.st_mode) && minmimal_list == 0) {
    printf("   %s\n",sous_chemin);
    open_and_list_dossier(sous_chemin);
}
else     printf("   %s\n",read_file_directory_dirent->d_name);
}

closedir(opened_file_directory);
return;
}

/*copy*/


void copy_file_to_destination_folder(char* entree, char* destination_path) {
    int nombre_octets_lu = 0;
    char reading_buffer[8096];
    int source = open(entree, O_RDONLY);
    char* entree_filename = basename(entree);

    char destination_file_path[256];
    sprintf(destination_file_path, "%s/%s", destination_path, entree_filename);

    int destination = open(destination_file_path, O_WRONLY | O_CREAT, S_IRUSR | S_IWUSR);
    while ((nombre_octets_lu = read(source, reading_buffer, sizeof(reading_buffer))) > 0) {
        if (write(destination, reading_buffer, nombre_octets_lu) != nombre_octets_lu) {
        }
    }
    close(source);
    close(destination);
    printf("SUCCESS, copied %s to %s\n", entree, destination_file_path);
}



//manque: check if folder exists, otherwise create folder
//
void  copy(char *entree, char *destination_path){
    struct dirent *read_entry_file_directory_dirent;
    stat(entree, &infos_entree );
    stat(destination_path, &infos_destination );
    DIR *opened_destination_directory = ouvre_dossier(destination_path);
    if(!S_ISDIR(infos_entree.st_mode)){
        copy_file_to_destination_folder(entree,destination_path);
        return;
    }
    // Étape 0 : ouverture du fichier/dossier d'entrée, ouverture du fichier/dossier de destination
    DIR *opened_entry_directory = ouvre_dossier(entree);
    // Gestion des cas nuls
    if (opened_entry_directory == NULL) {
        printf("Chemin du dossier d'entrée incorrect\n");
        return;
    }
    //Lacks: same permissions
    // Étape 1 : lecture des fichiers de manière récursive
    char sous_chemin_entree[120];
    sprintf(sous_chemin_entree, "%s/%s", entree, read_entry_file_directory_dirent->d_name);
    stat(sous_chemin_entree, &infos_entree );
    char sous_chemin_destination[120];
    stat(sous_chemin_entree, &infos_destination );
    //la destination dans ce cas est un dossier
    while ((read_entry_file_directory_dirent = readdir(opened_entry_directory)) != NULL) {
            if (read_entry_file_directory_dirent->d_name[0] == '.') //on va ignorer les fichiers cachés
            continue;
        sprintf(sous_chemin_entree, "%s/%s", entree, read_entry_file_directory_dirent->d_name);
        stat(sous_chemin_entree, &infos_entree);
        if (!S_ISDIR(infos_entree.st_mode)) {
            copy(sous_chemin_entree, destination_path);
        }
        else {
            sprintf(destination_path, "%s/%s", destination_path, read_entry_file_directory_dirent->d_name);
            printf("destination_path %s\n", destination_path);
        if (!mkdir(destination_path, S_IRWXU) && ouvre_dossier(destination_path)==NULL) {
            printf("ERROR CREATING FOLDER!\n");
            return;
        }
}}}


int main(int argc,char ** argv){
    char* path_dossier= argv[1] ;

    if (argc >= 3) {
        if (strcmp(argv[2], "-l") == 0)
        minmimal_list = 1;
        else {
            char* destination = argv[2];
            copy(path_dossier, destination);}}

    else{
    printf("\nfile path is %s\n", argv[1]);
    open_and_list_dossier(path_dossier);}
    /*/La sortie est donc similaire à la fonction ls et elle contient:
 le type de fichier: d pour un dossier, - pour un fichier régulier, l pour un lien; d'autre peuvent être implémentés;
 les droits en lecture/écriture/exécution: les droits d'accès ne concernent que les droits basiques mentionnés; les bits setuid/setgid, sticky bit et autres ne seront pas représentés;
 la taille du fichier en octets;
 la date de modication: voir la fonction strftime;
 le nom du fichier: donné relativement au dossier/fichier passé en paramètre.
La plus part des informations sont disponibles par les commandes lstat/stat/fstat vues en cours.*/
    return 0;
    }