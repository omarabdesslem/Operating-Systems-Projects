#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h> //pour uint
#include <stdlib.h> //malloc
#include <errno.h>


void suggerer_aide_boucle();
void print_help();
void  gerer_status(int status, char cmd, struct flock *flock_struct);
int essayer_fcntrl_et_interpréter(int descripteur_de_fichier, int cmd, int type, int start, int length, int whence);
void faire_verrouillage_deverrouillage(int descripteur_de_fichier_ouvert);