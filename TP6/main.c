#include <stdio.h>
#include <string.h> //pour strtok et strcmp
#include <unistd.h> //pour getpid(), getcwd()
#include <stdlib.h> //malloc
#include "jobs.c"
#include "builtin.c"

int MODE_DEBUG = 0;
char Chemin_de_Current_Working_Directory[256];

int gérer_entree_analyse(int argc_s, char **argv_s){
    if (strcmp(argv_s[0], "cd")==0){
        cd(argc_s,argv_s);
    }
    else if (strcmp(argv_s[0], "DEBUG")==0 || strcmp(argv_s[0], "debug")==0){
        MODE_DEBUG = 1;
        printf("MODE DEBUG Activée, DRAPEAU DEBUG EGALE À = %d\n", MODE_DEBUG);

    }
    else if (strcmp(argv_s[0], "MINIMAL")==0 || strcmp(argv_s[0], "light")==0){
        MODE_DEBUG = 0;
        printf("MODE DEBUG Désactivée, DRAPEAU DEBUG EGALE À = %d\n", MODE_DEBUG);
    }

    else if (strcmp(argv_s[0], "exit")==0 || strcmp(argv_s[0], "quit")==0){
        printf("\x1b[32mMerci d'avoir utilisé le shell. Au revoir et à bientôt! :)\x1b[0m\n");
        raise(SIGHUP);
    }
    else if (strcmp(argv_s[0], "help")==0 || strcmp(argv_s[0], "-h")==0){
        printf("Bienvenue au shell!\nCommands Builtin: cd | exit\n  ");
    }
    else if(strcmp(argv_s[argc_s-1], "&")==0) executerCommandes(argc_s,argv_s, 1, MODE_DEBUG);
    else if(strcmp(argv_s[argc_s-1], "&")!=0) executerCommandes(argc_s,argv_s, 0, MODE_DEBUG);
    return 0;


    }





int lire_entree(){
    char ligne_entree_utilsateur[256];
    char * argv_tableau_chaine_to_process_en_shell[256];
    memset(ligne_entree_utilsateur, 0, sizeof(ligne_entree_utilsateur));
    memset(argv_tableau_chaine_to_process_en_shell, 0, sizeof(argv_tableau_chaine_to_process_en_shell));
    if (MODE_DEBUG)
        printf("retourné à fct lire_entree(ignore)");
    int argc_shell=0;
    fflush(stdin);
    fgets(ligne_entree_utilsateur, sizeof(ligne_entree_utilsateur), stdin);
    //char* strtok(char *restrict string_d_entree, const char *restrict seperateur); 
    //remplace un token par NUL, et retourne un pointeur vers prochain token dans le string_d_entree
    size_t input_len = strlen(ligne_entree_utilsateur);
    if (input_len > 0 && ligne_entree_utilsateur[input_len - 1] == '\n') {
        ligne_entree_utilsateur[input_len - 1] = '\0';
    }
    char *token = strtok(ligne_entree_utilsateur, " ");
    while (token != NULL){
        argv_tableau_chaine_to_process_en_shell[argc_shell] = token;
        if( MODE_DEBUG == 1)
            printf("tableau[%d] %s\n",argc_shell, argv_tableau_chaine_to_process_en_shell[argc_shell]);
        token = strtok(NULL, " ");
         argc_shell+=1; 
         }
    gérer_entree_analyse(argc_shell, argv_tableau_chaine_to_process_en_shell);
    return 0;
}
int main(int argc, char* argv[]){
    /*struct sigaction {
  void *sa_handler (int);
  void *sa_sigaction (int, siginfo_t *, void*);
  sigset_t sa_mask;
  int sa_flags;}*/
    struct sigaction struct_assossier_signal_action;
    //on commence par créer un set de signaux, et ajouter les signaux qu'on veut manipuler
    sigset_t * set_de_signaux_a_masquer = &struct_assossier_signal_action.sa_mask; //passage_par_reference
    sigemptyset(set_de_signaux_a_masquer); //initisation à vide
    sigaddset(set_de_signaux_a_masquer, SIGTERM);
    sigaddset(set_de_signaux_a_masquer, SIGQUIT);
    if (MODE_DEBUG)
        printf("%u\n", struct_assossier_signal_action.sa_mask);
    struct_assossier_signal_action.sa_sigaction = gerer_signaux; //ASSOCIER UNE ACTION À UN SIGNAL
    //À refaire
    sigemptyset(&struct_assossier_signal_action.sa_mask);
    sigaddset(&struct_assossier_signal_action.sa_mask, SIGCHLD);
    sigaddset(&struct_assossier_signal_action.sa_mask, SIGINT);
    sigaddset(&struct_assossier_signal_action.sa_mask, SIGHUP);
    struct_assossier_signal_action.sa_flags = SA_SIGINFO | SA_RESTART ;
    sigaction(SIGCHLD, &struct_assossier_signal_action, NULL);
    sigaction(SIGINT, &struct_assossier_signal_action, NULL);
    sigaction(SIGHUP, &struct_assossier_signal_action, NULL);
    printf("\x1b[34m\n************** Bienvenue à mon SHELL **************\nTaper HELP pour aide, EXIT pour fermer le programme\x1b[0m\n");
    while(1){
        //getcwd(char* buffer, size_t taille_du_buffer)
        getcwd(Chemin_de_Current_Working_Directory, 256);
        fflush(stdout);
        printf("\x1b[35m\n[PID %d]>%s>\x1b[0m",getpid(),Chemin_de_Current_Working_Directory);
        lire_entree();
        }
    return 0;
}