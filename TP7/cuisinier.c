#include <sys/mman.h>
#include <unistd.h>
#include <stdio.h>
#include <semaphore.h>
#include <string.h>
#include "shm.h"

int DEBUG = 0;
sem_t * semaphore_pour_informer_le_serveur_de_servir, *semaphore_le_chef_se_repose, *semaphore_utilisation_exclusif_de_l_étagére;


void attendre_que_le_serveur_travail(sharedMemory* mémoire_partagée){
    while(mémoire_partagée->est_prête != prête ) sleep(1);
}


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

void check_si_chef_prend_une_pause(sharedMemory *mémoire_partagée_représentant_l_étagére_de_pizzas){
           if (mémoire_partagée_représentant_l_étagére_de_pizzas->nombre_de_pizza == 3) {
                printf("\x1B[33mLe chef se repose, Pizzas sur l'étagére = %ld\x1B[0m\n", mémoire_partagée_représentant_l_étagére_de_pizzas->nombre_de_pizza);
            sem_wait(semaphore_le_chef_se_repose);
        }
}

void chef_cuisine(sharedMemory *mémoire_partagée_représentant_l_étagére_de_pizzas){
    printf("\x1B[33mLe Chef commence à cuisiner.\x1B[0m\n");
    int compteur_de_pizza;
    for(compteur_de_pizza = 0; compteur_de_pizza < MAX_PIZZA; ) {
        if(mémoire_partagée_représentant_l_étagére_de_pizzas->nombre_de_pizza<3){
        sleep(rand()%5+1);
        //opération atomique de décrémentation de la valeur du sémaphore, dans ce cas bloquera l'accés pour les autres processus 
        sem_wait(semaphore_utilisation_exclusif_de_l_étagére);
        mémoire_partagée_représentant_l_étagére_de_pizzas->nombre_de_pizza++;
        printf("\x1B[35mChef a bien cuisiné une pizza. Pizzas disponible sur l'étagére: %ld\x1B[0m\n", mémoire_partagée_représentant_l_étagére_de_pizzas->nombre_de_pizza);
        //incrémente ==> débloque
        sem_post(semaphore_utilisation_exclusif_de_l_étagére);
        //++ pour informer le serveur de la nouvelle pizza disponible
        sem_post(semaphore_pour_informer_le_serveur_de_servir);
        compteur_de_pizza++;
        check_si_chef_prend_une_pause(mémoire_partagée_représentant_l_étagére_de_pizzas);}
    }
    printf("Chef a fini pour la journée.\n");

}
void detruire_la_mémoire_et_supprimer_référence(sharedMemory *mémoire_partagée_représentant_l_étagére_de_pizzas){
if(munmap(mémoire_partagée_représentant_l_étagére_de_pizzas, sizeof(sharedMemory)) == -1) //detruire la mémoire, ==> libération des ressources
        fprintf(stderr, "munmap");
shm_unlink("/gerer_le_restaurant");//uniquement la référence sera supprimée
}



int main(int argc, char* argv[]){

sharedMemory *mémoire_partagée_représentant_l_étagére_de_pizzas;
printf("\x1b[34m\n************** Processus Cuisinier Ouvert! **************\n");
// I. CRÉATION DE MÉMOIRE PARTAGÉE: le descripteur de fichier représente directement une mémoire physique
int descripteur_fichier_de_la_mémoire_partagée = shm_open("/gerer_le_restaurant", O_RDWR | O_CREAT);
if (DEBUG) printf("\x1B[32mMemoire partagée créer\x1B[0m\n");
//nom n’est pas un nom de fichier standard==> doit commencer par "/",  on retrouvera la mémoire partagée dans /dev/shm/name


// II. TAILLE DE LA MÉMOIRE: il faut donner une taille à l'objet de mémoire partagée avant son utilisation
ftruncate(descripteur_fichier_de_la_mémoire_partagée, sizeof(sharedMemory));
if (DEBUG) printf("\x1B[32mTruncated Memory\x1B[0m\n");
// III. (Objet) Mémoire partagée <---> Mémoire virtuelle du processus

//void *mmap(void *addr, size_t length, int prot, int flags, int fd, off_t offset);
mémoire_partagée_représentant_l_étagére_de_pizzas=mmap(NULL /*pour laisser sys d'exploitation choisir automatiquement une adresse*/,sizeof(sharedMemory), PROT_READ | PROT_WRITE, MAP_SHARED, descripteur_fichier_de_la_mémoire_partagée, 0 ); 
//mmap associe un ségment de mémoire virtuelle à un ségment de fichier , (Les parametres doivent correspondres au mode d'ouverture de l'objet POSIX)
if (DEBUG) printf("\x1B[32mAssocier l'Objet de mémoire partagé à la mémoire virtuelle du processus cuisinier\x1B[0m \n");

mémoire_partagée_représentant_l_étagére_de_pizzas->nombre_de_pizza = 0;


if (DEBUG) printf("\x1B[32mArriver à attendre_que_le_serveur_travail\x1B[0m\n");
mémoire_partagée_représentant_l_étagére_de_pizzas->est_prête = 1;
printf("\x1B[32mEn attente du serveur\x1B[0m\n");
attendre_que_le_serveur_travail(mémoire_partagée_représentant_l_étagére_de_pizzas);

initialiser_les_sémaphores();
if (DEBUG) printf("\x1B[initialiser_les_sémaphores\x1B[0m\n");
chef_cuisine(mémoire_partagée_représentant_l_étagére_de_pizzas);
if (DEBUG) printf("\x1B[chef_cuisine\x1B[0m\n");
fermer_et_destruire_semaphores();

detruire_la_mémoire_et_supprimer_référence(mémoire_partagée_représentant_l_étagére_de_pizzas);

return 0;
}
