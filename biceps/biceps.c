#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <readline/readline.h>
#include <readline/history.h>
#include <signal.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <bits/waitflags.h>
#include "gescom.h"

char* HIST_FILE = "hist_file";
int Mots_base_size = 64;

int Mots_size = 64;
static char ** Mots; /* le tableau des mots de la commande */
static int NMots = 0; /* nombre de mots de la commande */
char* save;

char *header;
char *input;
char *command;

/* Def */
char* head(void);
void interrupt(int S);
int analyseCom(char *b);
void freeMots(void);

/* Fonctions */

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

int analyseCom(char *b){
	
	char* out;
	int cnt = 0;
	
	if (Mots == NULL) {
        	Mots = malloc(sizeof(char *) * Mots_base_size);
        	Mots_size = Mots_base_size;
    	}
    	
	while((out = strsep(&b, " \t\n")) != NULL){
		if(*out=='\0') continue;
		Mots[cnt] = strdup(out);
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



