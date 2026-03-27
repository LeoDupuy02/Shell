#include "biceps.h"

/* Gestion des commandes */

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

/* Commandes */

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

void freeMots(void){
	for(int i = 0; i<NMots; i++){
		free(Mots[i]);
		Mots[i] = NULL;
	}
	free(Mots);
	Mots = NULL;
}
 
char* head(void){

	char *env = "USER";
	char *name;
	char hostname[32];
	char type;
	
	name = getenv(env);

	if(hostname != NULL){
	
		int exit = gethostname(hostname, 32);
		if(exit<0){
			perror("gethostname");
			return NULL;
		}
		if(strcmp(hostname,"root")==0){
			type = '#';
		}
		else{
			type = '$';
		}
		/* protection */
		hostname[31] = '\0';
	}
	else{
		perror("getenv");
		return NULL;
	}
	
	/* Malloc */
	int i = 0;
	int N= strlen(name)+strlen(hostname)+4;
	char *header = (char *)malloc(sizeof(char)*N);
	for(i = 0; i<(int)strlen(name); i++){
		header[i] = name[i];
	}		
	header[i] = '@';
	for(i = strlen(name)+1; i<(int)(strlen(name)+1+strlen(hostname)); i++){
		header[i] = hostname[i-strlen(name)-1];
	}
	header[i] = type;
	header[i+1] = ' ';
		
	return header;	
}

/**
* Analyse des commandes
**/

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
	
	char* out;
	int cnt = 0;
	
	if (Mots == NULL) {
        	Mots = malloc(sizeof(char *) * Mots_base_size);
        	Mots_size = Mots_base_size;
    	}
    	
	while((out = strsep(&b, " \t\n")) != NULL){
		if(*out=='\0') continue;
		Mots[cnt] = copyString(out);
		cnt = cnt + 1;
		if(cnt >=Mots_size){
			Mots = realloc(Mots, (cnt+1)*sizeof(char *));
			Mots_size = cnt+1;
		}
	}
	
	
	Mots[cnt] = NULL;
	NMots = cnt;
	
	return cnt;
}

/**
* MAIN
**/
 
int main(void)
{
	
	signal(SIGINT,SIG_IGN);
	
	/* Génération du début de ligne */
	if((header=head())==NULL){
		free(header);
		return 1;
	}
	
	/* Initialisation du shell */
	read_history(HIST_FILE);	
	majComInt();
	
	/* Main loop */
	while(1){
		input = readline(header);
		if(input == NULL){
			printf("EOF exit\n");
			write_history(HIST_FILE);
			free(input);
			free(header);
			exit(0);
		}
		save = input;
		/* Gestion d'une série de commandes */
		while((command=strsep(&input,";"))!=NULL){
			add_history(command);
			/* Gestion d'une commande */
			int N = analyseCom(command);
			if(N>0){
				if(execComInt(NMots, Mots)){
					execComExt(Mots);
				}	
			}
			freeMots();
		}
		free(save);
	}

	return 0;
}



