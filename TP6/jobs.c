//Lorsque la commande tapée par l'utilisateur n'est pas builtin, shell executera la commande avec execve:
//int execve(const char *filename, char *const argv[], char *const envp[]);


#include <unistd.h>
#include <signal.h>
#include <fcntl.h>


pid_t pid_job, background_job,foreground_job;


void executerCommandes(int argc, char *argv_chaine_des_arguments[], int si_travail_arriére_plan_demande, int MODE_DEBUG){
pid_job = fork();
if (pid_job == -1) {
	perror("fork");
	return;
}
int resultat_d_execution_commande, status;
char * command;
if(pid_job > 0) {
	// Code du parent
	//travail
	char * command = argv_chaine_des_arguments[0];
	if (si_travail_arriére_plan_demande == 1){
		// Job en arrière-plan, ne pas attendre
		printf("Background Job Running (PID %d)\n", pid_job);
		background_job = pid_job;}
	//attendre que le background job finisse
	else {
		foreground_job = pid_job;
		waitpid(pid_job, &status, 0);
		printf("\x1b[32mForeground Job Exited\x1b[0m\n");
		foreground_job = 0;
		}
}

else if(pid_job == 0){
	if(si_travail_arriére_plan_demande == 0 ) {
		//resultat_d_execution_commande = execvp("ls", (char *[]){"ls", "..", NULL});
		command = argv_chaine_des_arguments[0];
		resultat_d_execution_commande= execvp(command, argv_chaine_des_arguments);
    	if( resultat_d_execution_commande< 0)
			printf("Erreur dans l'execution de commande en avant plan\nresultat d'execution: %d\nargv_foreground= %s %s\n",resultat_d_execution_commande, command, argv_chaine_des_arguments[1]);
		printf("Foreground Job Exited\n");
	}

	if(si_travail_arriére_plan_demande){
		// Code de l'enfant
		//travail
		char * command = argv_chaine_des_arguments[0];
		argv_chaine_des_arguments[argc-1]= NULL;
		// Rediriger l'entrée standard vers /dev/null
		int descripteur_de_fichier_nul_à_rediriger = open("/dev/null", O_WRONLY); 
		int resultat_de_redirection_de_stdin = dup2(descripteur_de_fichier_nul_à_rediriger, STDOUT_FILENO);
		if (descripteur_de_fichier_nul_à_rediriger < 0 || resultat_de_redirection_de_stdin < 0 ) { 
    		printf("Erreur en redirigeant stdout et stdin vers /dev/null"); exit(EXIT_FAILURE); }
		close(descripteur_de_fichier_nul_à_rediriger); //fermer le descripteur de fichier ouvert

		if(execvp(command, argv_chaine_des_arguments) < 0){
				printf("Erreur dans l'execution de la commande en arrière plan\n, argv_background[0]= %s\n",argv_chaine_des_arguments[0] );
		}
		}
	}
else printf("Erreur");// Error
}


void gerer_signaux(int signum, siginfo_t *siginfo, void *unused){
	//siginfo struct qui contient des informations supplémentaires associées au signal, notamment pid, si_signo(num du signal)
    switch(signum){
		//pas de fonction non-réentrantes dans le HANDLER !!!! (malloc, free, printf) 
        case SIGCHLD:
		//cas du SIGCHLD: généré par le système d'exploitation lorsqu'un processus fils se termine, qu'il soit arrêté ou qu'il se termine normalement
		// pour eviter les zombies
		// compare pid du signal de la struct siginfo avec le pid du bckgrnd job, fasse un wait c.a.d on attend que le proc se termine correctement
            if(siginfo->si_pid == background_job){
                int wstatus;
                waitpid(siginfo->si_pid, &wstatus, 0);
                background_job = 0;
    			write(STDOUT_FILENO, "\x1b[32m\nBackground job exited.\x1b[35m\nEntrée shell dans ton dossier courant>\x1b[0m", 78);
				//printf met le txt dans un buffer avant, fflush vide tampon de sortie et force l'écriture
				//write fonction ré-entrante, malloc, free, printf non , l'avantage c'est quelle affiche/imprime tout de suite sans être affecté par le tampon de sortie.
				//et pas d'effet de bord
            }

        case SIGINT:
            if(foreground_job > 0){
                kill(foreground_job, SIGINT);
            }
            break;
        case SIGHUP:
            if(foreground_job > 0){
                kill(foreground_job, SIGHUP);
            }
            if(background_job > 0){
                kill(background_job, SIGHUP);
            }
            exit(EXIT_SUCCESS);
        default:
            break;
    }
}