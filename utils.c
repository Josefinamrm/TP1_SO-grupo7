
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

int safe_fork()
{
    int pid = fork();
    if (pid == -1)
        exit_failure("fork");
    return pid;
}

void safe_dup2(int src_fd, int dest_fd)
{
    printf("%d %d", src_fd, dest_fd);
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
