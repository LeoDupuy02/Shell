#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <readline/readline.h>
#include <readline/history.h>
#include <signal.h>


/* Globales */
int Mots_base_size = 64;
int Mots_size = 64;
static char ** Mots; /* le tableau des mots de la commande */
static int NMots; /* nombre de mots de la commande */

/* Définitions */
char* head(void);
void interrupt(int S);
char* copyString(char *s);
int analyseCom(char *b);

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
	char *val = (char *)malloc(sizeof(char)*(N));
	for(int i=0; i<N; i++){
		val[i]=s[i];
	}
	//val[N]='\0';
	
	return val;
}

int analyseCom(char *b){
	
	// Mots memory preparation
	for(int i = 0; i<Mots_size; i++){
		free(Mots[i]);
	}
	free(Mots);
	Mots = (char **)malloc(sizeof(char *)*Mots_base_size);
	
	printf("Start analyse command\n");
	
	char* out = "vide";
	int cnt = 0;
	
	while((out = strsep(&b, " ")) != NULL){
		Mots[cnt] = copyString(out);
		cnt = cnt + 1;
		if(cnt >=Mots_size){
			Mots = realloc(Mots, cnt+1);
			Mots_size = cnt+1;
		}
		printf("Mot %d : %s\n", cnt, out);
	}
	
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
	
	while(1){
		printf("%s ", header);
		command = readline(NULL);
		int N = analyseCom(command);
		printf("%d mots typed\n", N);
	}
	
	free(header);

	return 0;
}



