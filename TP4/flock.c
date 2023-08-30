#include "flock.h"

int  DEBUG = 0, compteur_sortir_gracieusement = 0, compteur_limite = 20;


//pour indiquer le fonctionnement du prog au lancement
void suggerer_aide_boucle(){
    char caractere_insérer_par_l_utilisateur = 'a';
    while (caractere_insérer_par_l_utilisateur != '?') {
        printf("Type ? for Help\n==> ");
        scanf(" %c", &caractere_insérer_par_l_utilisateur);
}}

void print_help(){
    suggerer_aide_boucle();
    printf("\nFormat: cmd l_type start length [whence(optional)]\n’cmd’ --- ’g’ (F_GETLK), ’s’ (F_SETLK), or ’w’ (F_SETLKW)\n’l_type’ --- ’r’ (F_RDLCK), ’w’ (F_WRLCK), or ’u’ (F_UNLCK)\n’start’ --- lock starting offset\n’length’ --- number of bytes to lock\n’whence’ --- ’s’ (SEEK_SET, default), ’c’ (SEEK_CUR), or ’e’ (SEEK_END)\n\n");
}


void  gerer_status(int status, char cmd, struct flock *flock_struct){

 if (cmd == F_GETLK) { /* F_GETLK*/ 
    printf("Commande = F_GETLK\n");
    if (status ==0){
        if (flock_struct->l_type == F_UNLCK )//la réponse est que flock_struct est unlocked?
            printf("[PID=%ld] Lockable\n",(long)getpid());
        else // range is occupied by another process
            printf("[PID=%ld] Locked by [PID=%d]\n",(long)getpid(),flock_struct->l_pid);
    }}

 else { /* F_SETLK, F_SETLKW */
    printf("Commande = F_SETLK ou F_SETLKW \n");
    if (status == 0 ){
        printf("[PID=%ld] Got lock\n",(long)getpid());//the fcntl() call goes well
    }
    else{return;}
// check status and handle errors (look at manual for possible errors) if (status ==0 ){
// process results and print informative text }else if (errno == SOME_ERROR){
// process results and print informative text }
}}



int essayer_fcntrl_et_interpréter(int descripteur_de_fichier, int cmd, int type, int start, int length, int whence){
    //initialisation_flock_structure
    //Remarques: commandes F_GETLK, F_SETLK, F_UNLCK etc sont passé en 2ème paramètre à fcntrl(), 
    //le reste des paramètres sont passé à la struct(l_type, whence,etc) (05/07)
    struct flock struct_flock;
    struct_flock.l_type = type;
    struct_flock.l_whence = whence;
    struct_flock.l_start = start;
    struct_flock.l_len = length;
    if (DEBUG) printf("Créer flock structure\n");
    int status = fcntl(descripteur_de_fichier, cmd, &struct_flock); //appel système  de type Standard C library (libc)
    if (DEBUG) printf("Initialiser fcntl\n");
    //partie d'interprétation du résultat
    gerer_status(status, cmd,  &struct_flock);
    return 0;}

short int assigner_lettre_a_valeur(char lettre, char* tableau_de_lettre, short int* tableau_de_valeur){
    for (int compteur=0; compteur < strlen(tableau_de_lettre); compteur++){
        if (tableau_de_lettre[compteur] == lettre) return tableau_de_valeur[compteur];
    }
    printf("Argument non existant.     Argument donné = %c, tableau = %c%c%c\n", lettre, tableau_de_valeur[0], tableau_de_valeur[1], tableau_de_valeur[2]);
    return 0;
}

void faire_verrouillage_deverrouillage(int descripteur_de_fichier_ouvert){
    compteur_sortir_gracieusement += 1;
    char lettre_du_cmd, lettre_du_type, lettre_du_whence='A';
    int start, length;
    struct flock flock_struct;
    int taille_max_de_la_ligne = 16348;
    char ligne_a_stocker_entree_standard_et_la_decomposer[taille_max_de_la_ligne];
 
    fflush(stdout); /*pour forcer le vidage du buffer de la sortie standard ==>écrire immédiatement toutes les données qui était dans le buffer*/
    if (compteur_sortir_gracieusement > 1){
    printf("PID=%ld> ", (long)/*forcer type*/ getpid()); //gets PID du processus en cours 
    /*scanf("%c %c %d %d",); pas utile dans ce cas,car on aura un argument en option 
    ==> fgets(char ligne_a_stocker[], int taille_maximale < 16384 ,source_entree (c.a.d le canal), on va choisir le canal standard stdin); (06.07.2023)
    */
    //scanf("%c",ligne_a_stocker_entree_standard_et_la_decomposer );
        if (fgets(ligne_a_stocker_entree_standard_et_la_decomposer,taille_max_de_la_ligne,stdin) != NULL) { /* retourne le resultat dans ligne_a_stocker, mais elle fait quoi avec (06.07.2023)? 
        ==> on va la décomposer et assigner chaque partie aux elements de flock avec sscanf
        ==> sscanf, scan une ligne existante selon un format choisit, idéalement sscanf(const char *restrict string_d_entree, const char *restrict format, variable_ou_stocker);
        */
        int nombre_arguments_assignés = sscanf(ligne_a_stocker_entree_standard_et_la_decomposer, "%c %c %d %d %c\n", &lettre_du_cmd, &lettre_du_type, &start, &length, &lettre_du_whence);
        short int cmd = assigner_lettre_a_valeur(lettre_du_cmd, "gsw", (short int[]){F_GETLK, F_SETLK, F_SETLKW});
        short int type= assigner_lettre_a_valeur(lettre_du_type, "rwu", (short int[]){F_RDLCK, F_WRLCK, F_UNLCK});
        //variable optionnelle
        short int whence = SEEK_SET;
        if (lettre_du_whence != 'A'){
            whence= assigner_lettre_a_valeur(lettre_du_whence, "sce", (short int[]){SEEK_SET, SEEK_CUR, SEEK_END});}
        //start, length reste les mêmes
        //aprés qu'on assigner tout les valeurs, on procéde au lock
        essayer_fcntrl_et_interpréter(descripteur_de_fichier_ouvert, cmd, type, start, length, whence);
    }
}
else {fgets(ligne_a_stocker_entree_standard_et_la_decomposer,taille_max_de_la_ligne,stdin);}

}

int main(int argc, char* argv[]){
    print_help();
    if (argc<2) return 0;
    if(argc=4 && (argv[3]=="Debug"|| argv[3]=="debug"|| argv[3]=="-d")  DEBUG=1; 
    char* chemin_fichier = argv[1];
    int descripteur_de_fichier_ouvert = open(chemin_fichier, O_RDWR,O_TRUNC, S_IRUSR|S_IWUSR); //ouvrir le fichier avec les permissions write, truncate, ReadUSR, WriterUSR(ca.d l'uttilisateur aura ces permissions)
    if (descripteur_de_fichier_ouvert == -1) {
        printf("Mauvais nom de fichier\n");
        return 0;}
    while (compteur_sortir_gracieusement<compteur_limite)
        faire_verrouillage_deverrouillage(descripteur_de_fichier_ouvert);
    return 0;
}