#include <unistd.h>
#include <signal.h>
pid_t pid_job, pid_travail_background;

void split_chaine_retourne_existance_et_position(int argc, char* argv[], int position_de_and_si_existante, char* argv_foreground[], char* argv_background[], int MODE_DEBUG) {
  int compteur_pour_le_tableau_arriére_plan = 0; // nécessaire pour ne pas confondre avec le compteur i

    // Copie les arguments avant le "&" dans argv_foreground
    for (int position_dans_le_tableau_principale_argv = 0; position_dans_le_tableau_principale_argv < position_de_and_si_existante; position_dans_le_tableau_principale_argv++) {
        argv_foreground[position_dans_le_tableau_principale_argv] = strdup(argv[position_dans_le_tableau_principale_argv]);
		if( MODE_DEBUG == 1)
			printf("argv_position copié jusqu'à %d \n", position_dans_le_tableau_principale_argv);
		// strdup copie dynamiquement le sous_string (je pense), un free récursif est nécessaire pour les deux strings à la fin
    }
	argv_foreground[position_de_and_si_existante]= NULL;

    // Copie les arguments après le "&" dans argv_background
    for (int i = position_de_and_si_existante + 1; i < argc; i++) {
        argv_background[compteur_pour_le_tableau_arriére_plan++] = strdup(argv[i]);
		if( MODE_DEBUG == 1)
			printf("argv_position_background copié jusqu'à %d \nargv_background[compteur_pour_le_tableau_arriére_plan++] = %s\n", compteur_pour_le_tableau_arriére_plan, argv_background[0]);
    }
}




void executerCommandes(int argc, char *argv_chaine_des_arguments[], int si_travail_arriére_plan_demande, int position_de_and_si_existante, int MODE_DEBUG){
pid_job = fork();
char *argv_foreground[256]; //je peux pas juste mettre [position_de_and_si_existante] car position_de_and_si_existante peut être = 0
char *argv_background[256-position_de_and_si_existante];
int resultat_d_execution_commande;
char * command;
if(pid_job > 0) {
	// Code du parent
	int pid_parent=getpid();
	printf("Foreground Job Begun\n");
	//attendre que le job finisse
	pause();
	wait(&pid_job);
	printf("Foreground Job Exited\n");

}
else if(pid_job == 0){
	// Code de l’enfant
	int pid_enfant=getpid();
	//travail
	char * command = argv_chaine_des_arguments[0];
	if(si_travail_arriére_plan_demande == 1 ) {
		split_chaine_retourne_existance_et_position( argc, argv_chaine_des_arguments, position_de_and_si_existante, argv_foreground, argv_background, MODE_DEBUG);
		command = argv_foreground[0];
		//resultat_d_execution_commande = execvp("ls", (char *[]){"ls", "..", NULL});
		resultat_d_execution_commande = execvp(command, argv_foreground);
	}
	else {command = argv_chaine_des_arguments[0];
		resultat_d_execution_commande = execvp(command, argv_chaine_des_arguments);}
    if(resultat_d_execution_commande < 0){
			printf("Erreur dans l'execution de commande en avant plan\nresultat d'execution: %d\nargv_foreground= %s %s\n",resultat_d_execution_commande, command, argv_foreground[1]);
	}
	else printf("Commande bien exécuter\n");
	//fin

	if(si_travail_arriére_plan_demande){
		 pid_travail_background = fork();
		if (pid_travail_background >0){
			//travail du sous-parent / enfant du 1er
			printf("Background Job Begun\n");
			//handler pour que l'enfant en background ignore SIGINT
            struct sigaction sa;

            sa.sa_handler = SIG_IGN;
            sa.sa_flags = SA_RESTART;
            sigaction(SIGINT, &sa, NULL); 



			wait(&pid_travail_background);

			printf("Background job exited\n");
		}
		else if (pid_travail_background == 0) {
		// Code du petit enfant
		int pid_enfant=getpid();
		//travail
		char * command = argv_background[0];
		// Rediriger l'entrée standard vers /dev/null
        freopen("/dev/null", "r", stdin);

		if(execvp(command, argv_background) < 0){
				printf("Erreur dans l'execution de la commande en arrière plan\n, argv_background[0]= %s\n",argv_background[0] );
		}
		}


	
	}
}
else printf("Erreur");// Error

int background_job = pid_job;
}


void gerer_signaux(int signum, siginfo_t *info, void *mydata){
    switch(signum){
        case SIGCHLD:
            if(info->si_pid == pid_travail_background){
                int wstatus;
                waitpid(info->si_pid, &wstatus, 0);
                pid_travail_background = 0;
                write(STDOUT_FILENO, "\nBackground job exited\n", 24);
            }
        case SIGINT:
            if(pid_job > 0){
                kill(pid_job, SIGINT);
            }
            break;
        case SIGHUP:
            if(pid_job > 0){
                kill(pid_job, SIGHUP);
            }
            if(pid_travail_background > 0){
                kill(pid_travail_background, SIGHUP);
            }
            exit(EXIT_SUCCESS);
        default:
            break;
    }
}