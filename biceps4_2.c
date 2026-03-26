#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <readline/readline.h>
#include <readline/history.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/wait.h>

#define V "1.0"
/* Globales */
#define NBMAXC 10 /* Nb maxi de commandes internes */
int Mots_base_size = 64;
int Mots_size = 64;
static char ** Mots; /* le tableau des mots de la commande */
static int NMots; /* nombre de mots de la commande */
char *header;

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
int Changedir(int N, char *P[]);
int printWorkingDir(int N, char *P[]);
int Vers(int N, char *P[]);
void freeMots(void);

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

int Sortie(int N, char *P[]) { 
	free(header);
	freeMots();
	exit(0);
}

int Changedir(int N, char *P[]){
	int i = 0;
	if(N>1){
		printf("%s\n", P[1]);
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

void interrupt(int S)
{
	printf("Interrupt : %d\n", S);	
	free(header);
	freeMots();
	exit(1);
}

void freeMots(void){
	for(int i = 0; i<Mots_size; i++){
		free(Mots[i]);
	}
	free(Mots);
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
	
	NMots = cnt;
	
	return cnt;
}

int main(void)
{
	char *input;
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
		input = readline(NULL);
		while((command=strsep(&input,";"))!=NULL){
			int N = analyseCom(command);
			if(N>0){
				if(execComInt(NMots, Mots)){
					execComExt(Mots);
				}	
			}
		}
	}
	
	free(header);
	freeMots();

	return 0;
}



