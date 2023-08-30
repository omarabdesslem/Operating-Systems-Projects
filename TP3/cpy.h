#include <dirent.h>
#include <sys/syscall.h>     
#include <unistd.h>
#include <stdio.h>
#include <sys/stat.h> //for infos
#include <time.h> //pour imprimer le temps
#include <string.h>
#include <fcntl.h>
#include <libgen.h> //pourquoi? rappelles-toi ==> basename du path for the sprintf


DIR* ouvre_dossier(char * chemin);
void  copy_file_to_destination_folder(char *entree, char *destination_path);
void  copy(char *entree, char *destination_path);