#include "cpy.h"

DIR* ouvre_dossier(char * chemin){
     if (opendir(chemin)== NULL){
    printf("FILE NOT FOUND\n");
    return NULL;}

    DIR *dir = opendir(chemin);
    return dir;
}

void  copy_file_to_destination_folder(char *entree, char *destination_path){
    int nombre_octets_lu = 0;
    char reading_buffer[8096];
    int source = open(entree, O_RDONLY);
    
    // Create the destination file path
    char destination_folder[180];
    sprintf(destination_folder, sizeof(destination_folder), "%s/%s", destination_path, basename((char*)entree));
    
    int destination = open(destination_path, O_WRONLY|O_CREAT|O_TRUNC, S_IRUSR|S_IWUSR);
    
    while ((nombre_octets_lu = read(source, reading_buffer, sizeof(reading_buffer))) > 0) {
        if (write(destination, reading_buffer, nombre_octets_lu) != nombre_octets_lu) {
            close(source);
            close(destination);
            return;
        }
    }
    
    close(source);
    close(destination);
}

//manque: check if folder exists, otherwise create folder
//
void  copy(char *entree, char *destination_path){
    struct dirent *read_entry_file_directory_dirent;
    struct stat infos_entree, infos_destination;
    stat(entree, &infos_entree );
    stat(destination_path, &infos_destination );
    DIR *opened_destination_directory = ouvre_dossier(destination_path);
    if(!S_ISDIR(infos_entree.st_mode)){
        copy_file_to_destination_folder(entree,destination_path);
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
    sprintf(sous_chemin_destination, "%s/%s", destination_path, read_entry_file_directory_dirent->d_name);
    stat(sous_chemin_entree, &infos_destination );
    //la destination dans ce cas est un dossier
    if (S_ISDIR(infos_entree.st_mode)){
    while ((read_entry_file_directory_dirent = readdir(opened_entry_directory)) != NULL) {
        sprintf(sous_chemin_entree, "%s/%s", entree, read_entry_file_directory_dirent->d_name);
        stat(sous_chemin_entree, &infos_entree);
        if (S_ISDIR(infos_entree.st_mode)) {
            copy_file_to_destination_folder(sous_chemin_entree, destination_path);
        }
        else {
            sprintf(destination_path, "%s/%s", destination_path, read_entry_file_directory_dirent->d_name);
            if (!mkdir(destination_path, S_IRWXU)) {printf("ERROR CREATING FOLDER!"); return;}
            copy(sous_chemin_entree, destination_path);}
        }
}
}