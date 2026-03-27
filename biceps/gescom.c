/**
* Gestion des commandes 
* Ajout, affichage et exécution des commandes 
**/

#include "gescom.h"

int cmds_size = 0;
Command cmds[NBMAXC];

extern char* input;
extern char* header;
extern char* HIST_FILE;
extern void freeMots(void);

/* Commandes internes */

int Sortie(int N, char *P[]) { 
	write_history(HIST_FILE);
	free(input);
	free(header);
	freeMots();
	exit(0);
}

int Changedir(int N, char *P[]){
	int i = 0;
	if(N>1){
		i = chdir(P[1]);
	}
	else{
		i = chdir("~");
	}
	return i;
}

int printWorkingDir(int N, char *P[]){
	char buf[1024];
	getcwd(buf, 1024);
	printf("%s\n",buf);
	return 0;
}

int Vers(int N, char *P[]){
	printf("Version : %s\n", V);
	return 0;
}

/* Gestion */

void ajouteCom(char *nom, int (*fptr)(int argc, char *argv[])){
	if(cmds_size>=NBMAXC){
		exit(1);
	}
	
	Command mac;
	mac.nom = nom;
	mac.fptr = fptr;
	cmds[cmds_size]=mac;
	cmds_size += 1;
}

void majComInt(void){
	ajouteCom("exit",Sortie);
	ajouteCom("cd",Changedir);
	ajouteCom("pwd", printWorkingDir);
	ajouteCom("vers", Vers);
}

void listeComInt(void){
	for(int i = 0; i<cmds_size; i++){
		printf("%s ", cmds[i].nom);
	}
	printf("\n");
}

int execComInt(int N, char **P){
	for(int i = 0; i<cmds_size; i++){
		if(strcmp(P[0], cmds[i].nom)==0){
			cmds[i].fptr(N,P);
			return 0;
		}
	}
	return 1;
}

int execComExt(char **P){

	int pid;
	
	if ((pid = fork()) == -1) {
		perror("fork"); 
		return 1;
	}
	if (pid == 0) { /* code du fils */
		// printf("Je suis le fils !\n");
		execvp(P[0], P);
		perror("execvp");
		return 1;
	}
	else { /* code du pere */
		// printf("Je suis le pere !\n");
	
		pid_t w;
		pid_t wstatus;
		w = waitpid((pid_t)pid, &wstatus, WUNTRACED | WCONTINUED);
	
		if (w == -1) {
			perror("waitpid");
			exit(EXIT_FAILURE);
		}
		return 0;
	}
}
