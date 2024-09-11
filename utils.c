#include <semaphore.h>
#include "utils.h"




void exit_failure(char * message){
    perror(message);
    exit(EXIT_FAILURE);
}

char * safe_fgets(char * buffer, int size, FILE * file){
    char * aux = fgets(buffer, size, file);
    if(aux == NULL){
        exit_failure("fgets\n");
    }
    return aux;
}

FILE * safe_popen(char * command, char * type){
    FILE * aux = popen(command, type);
    if(aux == NULL){
        exit_failure("popen\n");
    }
    return aux;
}

pid_t safe_getpid(){
    pid_t aux = getpid();
    if(aux == -1){
        exit_failure("getpid\n");
    }
    return aux;
}