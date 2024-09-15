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

void exit_failure(char * message);
char * safe_fgets(char * buffer, int size, FILE * file);
FILE * safe_popen(char * command, char * type);
pid_t safe_getpid();

// Function that forks safely. Returns child fd if succesful, otherwise it ends execution of the program
int safe_fork();

// Function that dups safely
void safe_dup2(int src_fd, int dest_fd);

// Function that creates pipes safely
void safe_pipe(int pipefd[2]);

// Function that closes file descriptors safely
void safe_close(int fd);

// Function that redirects file descriptors
void redirect_fd(int src_fd, int dest_fd, int fd_close);

// Function that writes to the indicated file descriptor.
void write_to_fd(int fd, char *string);



#endif