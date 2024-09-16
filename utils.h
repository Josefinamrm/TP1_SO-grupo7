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
#define CANTSLAVES 2
#define INITIAL_FILES 2
#define BUFFER_LENGTH 1024





/*--------------------------------------------- Safe Functions ---------------------------------------------*/

void exit_failure(char * message);


char * safe_fgets(char * buffer, int size, FILE * file);


FILE * safe_popen(char * command, char * type);


pid_t safe_getpid();


int safe_fork();


void safe_dup2(int src_fd, int dest_fd);


void safe_pipe(int pipefd[2]);


void safe_close(int fd);


int safe_shm_open(const char * name, int flag, mode_t mode);


void safe_ftruncate(int fd, off_t length);


void safe_fstat(int fd, struct stat * buf);


char * safe_mmap(void *addr, size_t len, int prot, int flags, int fd, off_t offset);

void safe_munmap(void * addr, size_t len);

sem_t *  safe_sem_open(char * name);

void safe_sem_close(sem_t * sem);


/*--------------------------------------------- File Descriptor Management ---------------------------------------------*/
void redirect_fd(int src_fd, int dest_fd, int fd_close);


void write_to_fd(int fd, char *string);


void write_to_shm(char * dest, char * src, size_t size, sem_t * sem);




/*--------------------------------------------- Other Functions ---------------------------------------------*/

char * open_shm_object(const char *name, int open_flags, int map_flags);

char * create_shm_object(const char *name, int open_flags, int map_flags, off_t shm_size);


nfds_t ininitalize_slaves(struct pollfd *readable_fds, int * writeable_fds, char ** filenames, int * filenames_qtty);





#endif