#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <readline/readline.h>
#include <readline/history.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/wait.h>


/* Globales */
#define NBMAXC 10 /* Nb maxi de commandes internes */
int Mots_base_size = 64;
int Mots_size = 64;
static char ** Mots; /* le tableau des mots de la commande */
static int NMots; /* nombre de mots de la commande */

/* Définitions */
char* head(void);
void interrupt(int S);
char* copyString(char *s);
int analyseCom(char *b);
void ajouteCom(char *nom, int (*fptr)(int argc, char *argv[]));
void majComInt(void);
void listeComInt(void);
int execComInt(int N, char **P);
int execComExt(char **P);
int Sortie(int N, char *P[]);

/* Structures */
typedef struct _Command{
	char* nom;
	int (*fptr)(int argc, char *argv[]);
} Command;
int cmds_size = 0;
Command cmds[NBMAXC];

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
}

void listeComInt(void){
	for(int i = 0; i<cmds_size; i++){
		printf("%s ", cmds[i].nom);
	}
	printf("\n");
}

int execComInt(int N, char **P){
	for(int i = 0; i<cmds_size; i++){
		printf("%s - n\n", P[0]);
		if(strcmp(P[i], cmds[i].nom)==0){
			printf("Hello\n");
			cmds->fptr(N,P);
			return 0;
		}
	}
	return 1;
}

int execComExt(char **P){
	/*
	Cette fonction va :
	• créer un processus fils (man 2 fork),
	• qui va demander à exécuter le nouveau code (man 3 execvp),
	• pendant que le père attend la fin de son fils (man 2 waitpid).
	Il faut,
	*/
	
	/* creation d'un nouveau processus */
	
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

/* Commandes */

int Sortie(int N, char *P[]) { exit(0); }

void interrupt(int S)
{
	printf("Interrupt : %d\n", S);	
	exit(1);
}
 
char* head(void){

	char *env = "USER";
	char *name;
	char hostname[32];
	char type;
	
	name = getenv(env);

	if(hostname != NULL){
	
		int exit = gethostname(hostname, 32);
		if(exit>0){
			perror("gethostname");
			return NULL;
		}
		if(strcmp(hostname,"root")==0){
			type = '#';
		}
		else{
			type = '$';
		}
		
	}
	else{
		perror("getenv");
		return NULL;
	}
	
	/* Malloc */
	int i = 0;
	int N= strlen(name)+strlen(hostname)+3;
	char *header = (char *)malloc(sizeof(char)*N);
	for(i = 0; i<(int)strlen(name); i++){
		header[i] = name[i];
	}		
	header[i] = '@';
	for(i = strlen(name)+1; i<(int)(strlen(name)+1+strlen(hostname)); i++){
		header[i] = hostname[i-strlen(name)-1];
	}
	header[i] = type;
		
	return header;	
}

char* copyString(char *s){
	int N = strlen(s);
	char *val = (char *)malloc(sizeof(char)*(N+1));
	for(int i=0; i<N; i++){
		val[i]=s[i];
	}
	val[N]='\0';
	
	return val;
}

int analyseCom(char *b){
	
	// Mots memory preparation
	for(int i = 0; i<Mots_size; i++){
		free(Mots[i]);
	}
	free(Mots);
	Mots = (char **)malloc(sizeof(char *)*Mots_base_size);
	
	char* out = "vide";
	int cnt = 0;
	
	while((out = strsep(&b, " ")) != NULL){
		Mots[cnt] = copyString(out);
		cnt = cnt + 1;
		if(cnt >=Mots_size){
			Mots = realloc(Mots, cnt+1);
			Mots_size = cnt+1;
		}
	}
	Mots[cnt] = NULL;
	
	NMots = cnt;
	
	return cnt;
}

int main(void)
{
	char *header;
	char *command;
	Mots = (char **)malloc(sizeof(char *)*Mots_base_size);
	
	signal(SIGINT,interrupt);
	
	if((header=head())==NULL){
		free(header);
		return 1;
	}
	
	majComInt();
	listeComInt();
	
	while(1){
		printf("%s ", header);
		command = readline(NULL);
		int N = analyseCom(command);
		if(N>0){
			if(execComInt(NMots, Mots)){
				execComExt(Mots);
			}	
		}
	}
	
	free(header);

	return 0;
}



