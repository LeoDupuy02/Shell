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

/* Globales */
#define V "1.0"

#define NBMAXC 10
#define HIST_FILE "hist_file"
#define Mots_base_size 64

int Mots_size = 64;
static char ** Mots; /* le tableau des mots de la commande */
static int NMots = 0; /* nombre de mots de la commande */
char* save;

char *header;
char *input;
char *command;

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

/* Commandes */
typedef struct _Command{
	char* nom;
	int (*fptr)(int argc, char *argv[]);
} Command;
int cmds_size = 0;
Command cmds[NBMAXC];

