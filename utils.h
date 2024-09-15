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
#include <poll.h>
#include <errno.h>
#include <time.h>

#define TERMINATION "END"





/*--------------------------------------------- Safe Functions ---------------------------------------------*/

void exit_failure(char * message);


char * safe_fgets(char * buffer, int size, FILE * file);


FILE * safe_popen(char * command, char * type);


pid_t safe_getpid();


int safe_fork();


void safe_dup2(int src_fd, int dest_fd);


void safe_pipe(int pipefd[2]);


void safe_close(int fd);


void redirect_fd(int src_fd, int dest_fd, int fd_close);


void write_to_fd(int fd, char *string);


char * initialize_shm(const char * name, off_t shm_size);


nfds_t ininitalize_slaves(int cant_slaves, int initial_files, struct pollfd *readable_fds, int * writeable_fds, char ** filenames, int * filenames_qtty);








/*--------------------------------------------- Shared Memory ---------------------------------------------*/

#endif