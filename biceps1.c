#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <readline/readline.h>
#include <readline/history.h>
#include <signal.h>


/* Globales */
int console = 1;

/* Définitions */
char* head(void);
void interrupt(int S);

void interrupt(int S)
{
	printf("leo\n");	
	console = 0;
}
 
char* head(void){

	char *env = "USER";
	char name[32];
	char *hostname;
	char type;
	
	hostname = getenv(env);

	if(hostname != NULL){
	
		int exit = gethostname(name, 32);
		if(exit>0){
			perror("gethostname");
			return NULL;
		}
		if(hostname=="root"){
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
	for(i = 0; i<strlen(name); i++){
		header[i] = name[i];
	}		
	header[i] = '@';
	for(i = strlen(name)+1; i<strlen(name)+1+strlen(hostname); i++){
		header[i] = hostname[i-strlen(name)-1];
	}
	header[i] = type;
		
	return header;	
}

int main(void)
{
	char *header;
	char *command;
	int exit;
	
	signal(SIGINT,interrupt);
	
	if((header=head())==NULL){
		free(header);
		return 1;
	}
	
	while(console){
		printf("%s", header);
		command = readline(NULL);
	}
	
	free(header);

	return 0;
}



