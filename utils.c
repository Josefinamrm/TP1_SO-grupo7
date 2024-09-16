

#include "utils.h"


void exit_failure(char *message){
    perror(message);
    exit(EXIT_FAILURE);
}


char *safe_fgets(char *buffer, int size, FILE *file){
    char *aux = fgets(buffer, size, file);
    if (aux == NULL){
        exit_failure("fgets\n");
    }
    return aux;
}


FILE *safe_popen(char *command, char *type){
    FILE *aux = popen(command, type);
    if (aux == NULL){
        exit_failure("popen\n");
    }
    return aux;
}


pid_t safe_getpid(){
    pid_t aux = getpid();
    if (aux == -1){
        exit_failure("getpid\n");
    }
    return aux;
}


int safe_fork(){
    int pid = fork();
    if (pid == -1)
        exit_failure("fork");
    return pid;
}


void safe_dup2(int src_fd, int dest_fd){
    if (dup2(src_fd, dest_fd) == -1){
        exit_failure("dup2");
    }
}


void safe_pipe(int pipefd[2]){
    if (pipe(pipefd) == -1){
        exit_failure("pipe");
    }
}


void safe_close(int fd){
    char message[30];
    sprintf(message, "close fd %d", fd);
    if (close(fd) == -1)
    {
        exit_failure(message);
    }
}


void redirect_fd(int src_fd, int dest_fd, int fd_close){
    safe_close(fd_close);
    safe_dup2(src_fd, dest_fd);
    safe_close(src_fd);
}


void write_to_fd(int fd, char *string){
    size_t len = strlen(string) + 1;
    char buffer[len];
    sprintf(buffer, "%s\n", string);
    write(fd, buffer, len);
}


void write_to_shm(char * dest, char * src, size_t size, sem_t * sem){
    memcpy(dest, src, size + 1);
    sem_post(sem);
}


char *initialize_shm(const char *name, off_t shm_size){

    int shm_fd = shm_open(name, O_CREAT | O_RDWR, 0);

    if (shm_fd == -1){
        exit_failure("shm_open");
    }

    if (ftruncate(shm_fd, shm_size) == -1){
        exit_failure("ftruncate");
    }

    char *shm_ptr = mmap(NULL, shm_size, PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);
    if (shm_ptr == MAP_FAILED){
        exit_failure("mmap");
    }

    close(shm_fd);
    return shm_ptr;
}


nfds_t ininitalize_slaves(struct pollfd *readable_fds, int *writeable_fds, char **filenames, int *filenames_qtty){
    nfds_t open_read_fds = 0;
    for (int i = 0; i < CANTSLAVES; i++){
        // Create pipes, one for reading and one for writing
        int c2p_pipe[2];
        int p2c_pipe[2];

        safe_pipe(c2p_pipe);
        safe_pipe(p2c_pipe);

        int pid = safe_fork();
        if (pid == 0){
            for (int j = 0; j < open_read_fds; j++){
                safe_close(readable_fds[j].fd);
                safe_close(writeable_fds[j]);
            }

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
        writeable_fds[i] = p2c_pipe[1];

        for (int k = 0; k < INITIAL_FILES; k++, (*filenames_qtty)--, filenames++){
            write_to_fd(p2c_pipe[1], *filenames);
        }
    }

    return open_read_fds;

}

