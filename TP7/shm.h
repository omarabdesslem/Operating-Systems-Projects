#ifndef _SHM_H
#define _SHM_H

#include <stdlib.h>
#include <stdio.h>

//La mÃ©moire partagÃ©e contiendra deux valeur:
//- un indicateur stipulant si un autre processus est prÃ¨s pour l'opÃ©ration
//- un nombre qui sera incÃ©mentÃ© par les deux processus conjointement
typedef struct {
    char est_prête;
    long int nombre_de_pizza;
} sharedMemory;

#define prête 0
#define MAX_PIZZA 10


void OnError(const char *str)
{
    perror(str);
    exit(EXIT_FAILURE);
}

#endif