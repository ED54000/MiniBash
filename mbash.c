#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <assert.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <stdbool.h>

#define MAXLI 2048

// Arguments
char cmd[MAXLI];
char path[MAXLI];
char repCourant[MAXLI];
int pathidx;
char* historique[MAXLI];
int nbHistory = 1;


void mbash();
void rtrim(char* string, const char* element);
void execute(char* cmd);
char** split(const char* string, const char* delim);
bool exists(const char* path, const char * cmd);

int main(int argc, char** argv) {
  while (1) {
     
    getcwd(repCourant, sizeof(repCourant));
    printf("%s : ", repCourant);
    fgets(cmd, MAXLI, stdin);
    mbash(cmd);
  }
  return 0;
}

void mbash() {
    // Cas ou il n'y a pas de commandes
    if(cmd[0]=='\n'){

    }else{
        rtrim(cmd, "\n");
        // On entre la commande dans l'historique
        historique[nbHistory]=strdup(cmd);
        // On incrémante l'historique de 1
        nbHistory++;
        if (strstr(cmd, "cd") != NULL) {
            char* chemin;
            // Split du string pour obtenir le premier mot
            strtok(cmd, " ");
            // Split du deuxième string pour récupérer l'argument de la commande cd
            chemin = strtok(NULL, " ");
            // On vérifie que le chemin existe
            if (chemin == NULL){
    	        chemin = "/home/";
            }else {
                rtrim(chemin, "\n");
            }
        chdir(chemin);
        // Execution de la commande exit
        } else if (strstr(cmd, "exit") != NULL) {
            exit(1);
        // Execution de la commande history
        } else if (strcmp(cmd, "history") == 0) {
            for (int i=0; i<nbHistory; i++){
            printf("%d : %s\n", i, historique[i]);
        }
        // pour les commandes présentes dans le $PATH
        } else {
            char* path = getenv("PATH");
            char* buffer=malloc(MAXLI);
            char* temp = malloc(MAXLI);
            char* commande = strtok(strcpy(temp, cmd), " ");
            strcpy(buffer, path);
            char* s = strtok(buffer, ":");
            rtrim(s, "\n");
            bool existe;
            existe = exists(s,commande);
            //Teste si la commande existe
            while (existe == 0 && s!=NULL) {
                rtrim(s, "\n");
                existe = exists(s,commande);
                s = strtok(NULL, ":");
            }
            if ( (s = strtok(NULL, ":"))== NULL){
                printf("%s : Cette commande n'existe pas\n",commande);
            }else{
                execute(cmd);
            }
        }
    }
}

void rtrim(char* string, const char* element) {
  // On récupère la position à partir de laquelle il y a \n
  size_t posFin = strcspn(string , element);
  // On met le caractère de fin de string à la position trouvée
  string[posFin]='\0';
}

void execute(char* cmd) {
    // On sépare la commande des arguments
    char** options = split(cmd, " ");
    char* temp = malloc(MAXLI);
    char* prefixe = strtok(strcpy(temp, cmd), " ");
    free(temp);
    rtrim(prefixe, "\n");
    char* argv[MAXLI];
    int i = 0;
    // Copie du tableau d'options dans le type de donnée appropriée pour la fonction execvp
    while (options[i] != NULL) {
        argv[i] = strdup(options[i]);
        rtrim(argv[i],"\n");
        i++;
    }
    int pid = fork();
    if (pid == 0) {
        if (strstr(cmd, "&") != NULL) {
            options[i - 1] = NULL;  // Suppression du "&" de la liste des options
            execvp(prefixe, options);
        }else {
            execvp(prefixe, options);
        }
    }else {
        if (strstr(cmd, "&") != NULL) {
            // Si "&" est présent, ne pas attendre le processus fils
        } else {
            // Attendre le processus fils
            waitpid(pid, NULL, 0);
        }
    }
}

bool exists(const char* path, const char * cmd){
    char * chemin = malloc(MAXLI);
    strcpy(chemin,path);
    rtrim(chemin, "\n");
    strcat(chemin,"/");
    strcat(chemin, cmd);
    // Teste si la commande existe en essayant de l'ouvrir en lecture
    bool res = fopen(chemin,"r");
    free(chemin);
    return res;
}

char** split(const char* string, const char* delim) {
    char* buffer = malloc(MAXLI);
    // Copie du string en paramètres dans un buffer pour éviter la modification de l'original
    strcpy(buffer, string);
    // Création d'un tableau de String
    char** string_array = malloc(MAXLI);
    char* string_courant;
    string_courant = strtok(buffer, delim);
    rtrim(string_courant, "\n");
    int i = 0;
    // Tant qu'on a pas fini de traverser tous les Strings
    while (string_courant != NULL) {
        string_array[i] = strdup(string_courant);
        i++;
        string_courant = strtok(NULL, " ");
        if (string_courant != NULL) {
            rtrim(string_courant, "\n");
        }
    }
    return string_array;
}
