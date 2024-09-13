/*

- recibe por lineade comando los archivos a procesar
- iniciar procesos esclavos
- distribuir una cierta cantidad (significativsmente menor que el total) de archivos entre los esclavos
* Cuando un esclavo se libera, la aplicación le debe enviar un nuevo archivo para procesarlo
- debe recibir el resultado del procesamiento de cada archivo y lo agrefa agregarlo a un buffer por orden de llegada
- debe esperar 2seg a que aparezca un procesovista
- termina cuando todos los archivos estén procesados
- debe guardar le resultado en el archivo resultado (aparezca el proceso vista o no)

*/

#include <unistd.h>
#include <stdlib.h>
#include <sys/select.h>
#include <string.h>
#include <fcntl.h>
#include <stdio.h>
#include <sys/wait.h>
#include <poll.h>

#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/shm.h>

#include "utils.h"

#define CANTCHILD 1
#define INITIAL_FILES 2
#define BUFFER_LENGTH 1024
#define SHM_LENGTH 1024

// Function that exits with code EXIT_FAILURE and displays error message
void exit_failure(char *message);

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

int main(int argc, char *argv[])
{

    if (argc < 2)
    {
        printf("Program must be called with at least one file to analyse\n");
        exit(EXIT_FAILURE);
    }

    // Variables for file descriptors, used in IPC
    nfds_t open_read_fds = 0;
    struct pollfd *readable_fds = calloc(CANTCHILD, sizeof(struct pollfd));
    if (readable_fds == NULL)
    {
        exit_failure("calloc");
    }
    int write_fds[CANTCHILD];

    int shm_fd = shm_open("shm", O_CREAT | O_RDWR, 0);
    if(shm_fd == -1){
        exit_failure("shm_open");
    }
    if(ftruncate(shm_fd, SHM_LENGTH) == -1){
        exit_failure("ftruncate");
    }
    void *shm_ptr = mmap(NULL, SHM_LENGTH, PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);
    if(shm_ptr == MAP_FAILED){
        exit_failure("mmap");
    }

    semaphore * sem = malloc(sizeof(semaphore));
    if(sem == NULL){
        exit_failure("malloc");
    }
    sem->can_read = sem_open("can_read", O_CREAT, 0644, 0);


    // Make arguments ready for passing
    argc--;
    argv++;

    for (int i = 0; i < CANTCHILD; i++)
    {
        // Create pipes, one for reading and one for writing
        int c2p_pipe[2];
        int p2c_pipe[2];

        safe_pipe(c2p_pipe);
        safe_pipe(p2c_pipe);

        int pid = safe_fork();
        if (pid == 0)
        {

            redirect_fd(c2p_pipe[1], STDOUT_FILENO, c2p_pipe[0]);
            redirect_fd(p2c_pipe[0], STDIN_FILENO, p2c_pipe[1]);

            char *child_argv[] = {"./slave", NULL};
            execve("./slave", child_argv, NULL);
            exit_failure("execve\n");
        }

        close(c2p_pipe[1]);
        close(p2c_pipe[0]);

        readable_fds[i].fd = c2p_pipe[0];
        readable_fds[i].events = POLLIN;
        open_read_fds++;
        write_fds[i] = p2c_pipe[1];

        for (int i = 0; i < INITIAL_FILES && i <= argc; i++, argc--, argv++)
        {
            write_to_fd(p2c_pipe[1], *argv);
        }
    }

    int result;
    while (open_read_fds > 0)
    {
        result = poll(readable_fds, CANTCHILD, -1);
        if (result == -1)
        {
            exit_failure("poll");
        }

        for (int i = 0; i < CANTCHILD; i++)
        {
            if (readable_fds[i].revents != 0)
            {
                // Data available
                if (readable_fds[i].revents & POLLIN)
                {
                    char to_read;
                    int n, read_flag = 0, counter = 0;
                    char buffer[BUFFER_LENGTH];
                    while (!read_flag && (n = read(readable_fds[i].fd, &to_read, 1)) > 0)
                    {
                        // cambiar a \n
                        if (to_read == '\n')
                        {
                            buffer[counter] = '\0';
                            //  aca seria donde escribe la shared

                            memcpy(shm_ptr, buffer, counter+1);
                            sem_post(sem->can_read);
                            shm_ptr += counter+1;

                            //write(STDOUT_FILENO, buffer, counter+1);
                            //fflush(stdout);
                            
                            counter = 0;
                            read_flag = 1;
                            // si hay argumentos para mandarle le mando, sino no
                            if (argc > 0)
                            {
                                write_to_fd(write_fds[i], *argv);
                                argv++;
                                argc--;
                            }
                            else
                            {
                                if(write_fds[i] != -1){
                                    safe_close(write_fds[i]);
                                    write_fds[i] = -1;
                                }
                            }
                        }
                        else
                        {
                            buffer[counter++] = to_read;
                        }
                    }
                }
                // Error or eof
                else
                {
                    safe_close(readable_fds[i].fd);
                    readable_fds[i].fd = -1;
                    open_read_fds--;
                }
            }
        }
    }

    char * eof = "EOF";
    memcpy(shm_ptr, eof, sizeof(EOF));
    sem_post(sem->can_read);
    munmap(shm_ptr, SHM_LENGTH);
    close(shm_fd);
}

void exit_failure(char *message)
{
    perror(message);
    exit(EXIT_FAILURE);
}

int safe_fork()
{
    int pid = fork();
    if (pid == -1)
        exit_failure("fork");
    return pid;
}

void safe_dup2(int src_fd, int dest_fd)
{
    if (dup2(src_fd, dest_fd) == -1)
    {
        exit_failure("dup2");
    }
}

void safe_pipe(int pipefd[2])
{
    if (pipe(pipefd) == -1)
    {
        exit_failure("pipe");
    }
}

void safe_close(int fd)
{
    char message[30];
    sprintf(message, "close fd %d", fd);
    if (close(fd) == -1)
    {
        exit_failure(message);
    }
}

void redirect_fd(int src_fd, int dest_fd, int fd_close)
{
    safe_close(fd_close);
    safe_dup2(src_fd, dest_fd);
    safe_close(src_fd);
}

void write_to_fd(int fd, char *string)
{
    size_t len = strlen(string) + 1;
    char buffer[len];
    sprintf(buffer, "%s\n", string);
    write(fd, buffer, len);
}