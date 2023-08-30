#include <sys/mman.h>
#include <unistd.h>
#include <stdio.h>
#include <semaphore.h>
#include <string.h>
#include "shm.h"


sem_t * semaphore_pour_informer_le_serveur_de_servir,  * semaphore_le_chef_se_repose, * semaphore_utilisation_exclusif_de_l_étagére;

int DEBUG =0;

void initialiser_les_sémaphores(){
    //sem_open(const char *nom_unique, int oflag, mode_t mode, unsigned int value);

    semaphore_pour_informer_le_serveur_de_servir = sem_open("semaphore_pour_informer_le_serveur_de_servir", O_CREAT|O_RDWR, 0666, 0);
    // 0666 : c'est mode RW pour tout le monde: S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH
    semaphore_le_chef_se_repose = sem_open("semaphore_le_chef_cuisine", O_CREAT|O_RDWR, 0666, 0);
    semaphore_utilisation_exclusif_de_l_étagére = sem_open("semaphore_utilisation_exclusif_de_l_étagére", O_CREAT|O_RDWR, 0666, 1);

}


void fermer_et_destruire_semaphores() {
    //ferme que la référence à la sémaphore  et libère les ressources associées. 
    sem_close(semaphore_pour_informer_le_serveur_de_servir);
    sem_close(semaphore_le_chef_se_repose);
    sem_close(semaphore_utilisation_exclusif_de_l_étagére);
    //Après sem_close, la sémaphore elle-même reste disponible pour les autres processus qui l'utilisent, d'où sem_unlink

    //suppression de la sémaphore (du nom de la sémaphore techniquement)
    sem_unlink("semaphore_pour_informer_le_serveur_de_servir");
    sem_unlink("semaphore_le_chef_cuisine");
    sem_unlink("semaphore_utilisation_exclusif_de_l_étagére");
}

void check_si_chef_fini_sa_pause(sharedMemory *mémoire_partagée_représentant_l_étagére_de_pizzas){
        if (mémoire_partagée_représentant_l_étagére_de_pizzas->nombre_de_pizza < 3) sem_post(semaphore_le_chef_se_repose);      
}

void servir_les_pizzas(sharedMemory *mémoire_partagée_représentant_l_étagére_de_pizzas){
    printf("Commence à servir les pizzas.\n");
    int compteur_de_pizza = 0;
    while(compteur_de_pizza < MAX_PIZZA){
    if (mémoire_partagée_représentant_l_étagére_de_pizzas->nombre_de_pizza>0) {
        //décrémente le nombre de pizzas à servir
        sem_wait(semaphore_pour_informer_le_serveur_de_servir);
        sem_wait(semaphore_utilisation_exclusif_de_l_étagére);
        //temps pour servir une pizza
        sleep(rand()%7 +1);
        mémoire_partagée_représentant_l_étagére_de_pizzas->nombre_de_pizza--;
        printf("\x1B[36mServeur a bien servi une pizza. Pizzas disponible sur l'étagére: %ld\x1B[0m\n", mémoire_partagée_représentant_l_étagére_de_pizzas->nombre_de_pizza);
        //incrémente ==> débloque
        sem_post(semaphore_utilisation_exclusif_de_l_étagére);
        check_si_chef_fini_sa_pause(mémoire_partagée_représentant_l_étagére_de_pizzas);
        compteur_de_pizza++;
    }}

    printf("Serveur a fini pour la journée.\n");

}

void dissocier_la_mémoire_partagée(sharedMemory *mémoire_partagée_représentant_l_étagére_de_pizzas){
if(munmap(mémoire_partagée_représentant_l_étagére_de_pizzas, sizeof(sharedMemory)) == -1) //detruire la mémoire, ==> libération des ressources
        fprintf(stderr, "munmap");
}

int main(int argc, char* argv[]){
sharedMemory *mémoire_partagée_représentant_l_étagére_de_pizzas;
printf("\x1b[34m\n************** Processus Serveur Ouvert! **************\n");

int descripteur_fichier_de_la_mémoire_partagée;
// I. Ouverture de MÉMOIRE PARTAGÉE (Déjà créer): le descripteur de fichier représente directement une mémoire physique
//nom n’est pas un nom de fichier standard==> doit commencer par "/",  on retrouvera la mémoire partagée dans /dev/shm/name
while((descripteur_fichier_de_la_mémoire_partagée = shm_open("/gerer_le_restaurant", O_RDWR)) == -1){
        printf("\x1B[35mEn attente du Cuisinier.\x1B[0m\n");
        sleep(6);
}

//Truncate déjà fait en cuisinier.c

// II. (Objet) Mémoire partagée <---> Mémoire virtuelle du processus

//void *mmap(void *addr, size_t length, int prot, int flags, int fd, off_t offset);
mémoire_partagée_représentant_l_étagére_de_pizzas=mmap(NULL /*pour laisser sys d'exploitation choisir automatiquement une adresse*/,sizeof(sharedMemory), PROT_READ | PROT_WRITE, MAP_SHARED,descripteur_fichier_de_la_mémoire_partagée,0 ); 
//mmap associe un ségment de mémoire virtuelle à un ségment de fichier , (Les parametres doivent correspondres au mode d'ouverture de l'objet POSIX)
if (mémoire_partagée_représentant_l_étagére_de_pizzas->est_prête!= prête)
    mémoire_partagée_représentant_l_étagére_de_pizzas->est_prête = prête; 

initialiser_les_sémaphores();
servir_les_pizzas(mémoire_partagée_représentant_l_étagére_de_pizzas);
dissocier_la_mémoire_partagée(mémoire_partagée_représentant_l_étagére_de_pizzas);
return 0;
}


