#ifndef UTILS_H
#define UTILS_H

#include <semaphore.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/wait.h>
#include <sys/shm.h>
#include <sys/stat.h>
#include <sys/mman.h>



typedef struct {
    sem_t * mutex; //  = 1 !!!
    sem_t * can_read;
} semaphore;


void exit_failure(char * message);
char * safe_fgets(char * buffer, int size, FILE * file);
FILE * safe_popen(char * command, char * type);
pid_t safe_getpid();





#endif