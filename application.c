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


#define CANTCHILD 1
#define INITIAL_FILES 2
#undef max // lo saque de select_tut
#define max(x, y) ((x) > (y) ? (x) : (y))

// Function that exits with code EXIT_FAILURE and displays error message
void exit_failure(char *message);

// Function that forks safely. Returns child fd if succesful, otherwise it ends execution of the program
int safe_fork();

// Function that dups safely
void safe_dup2(int src_fd, int dest_fd);

// Function that redirects file descriptors
void redirect_fd(int src_fd, int dest_fd, int fd_close);






int main(int argc, char *argv[])
{

    if (argc < 2)
    {
        printf("Program must be called with at least one file to analyse\n");
        exit(EXIT_FAILURE);
    }


    // Variables for file descriptors, used in IPC
    fd_set read_fds_set;
    FD_ZERO(&read_fds_set);
    // chequear la otra forma para recorrer el fd_set
    int read_fds[CANTCHILD];
    int write_fds[CANTCHILD];
    int nfds = 0;


    // Make arguments ready for passing
    argc--;
    argv++;
    

    for (int i = 0; i < CANTCHILD; i++)
    {
        // Create pipes, one for reading and one for writing
        int c2p_pipe[2];
        int p2c_pipe[2];

        if (pipe(c2p_pipe) == -1 || pipe(p2c_pipe) == -1)
            exit_failure("pipe");

        int pid = safe_fork();
        if (pid == 0)
        {
        
            redirect_fd(c2p_pipe[1], STDOUT_FILENO, c2p_pipe[0]);
            redirect_fd(p2c_pipe[0], STDIN_FILENO, p2c_pipe[1]);

            // ahora debería de tener
            // 0 -> read end del p2c_pipe
            // 1 -> write end del c2p_pipe
            // 2-> tty

            char *child_argv[] = {"./slave", NULL};
            execve("./slave", child_argv, NULL);
            exit_failure("execve\n");
        }

        close(c2p_pipe[1]);
        close(p2c_pipe[0]);

        FD_SET(c2p_pipe[0], &read_fds_set);
        read_fds[i] = c2p_pipe[0];
        write_fds[i] = p2c_pipe[1];

        nfds = max(nfds, c2p_pipe[0]);

        for(int i = 0; i < INITIAL_FILES; i++, argc--){
            write(p2c_pipe[1], *argv, strlen(*argv)+1);
        }
    }


    // select(nfds+1, copy_read_fds, NULL, NULL, NULL, NULL) = 2
    fd_set copy_read_fds_set;
    FD_ZERO(&copy_read_fds_set);

    

    // deberia de chequear la condicion de este loop ya que puede que en el interior se deje de cumplir y me rompa el programa
    while (argc)
    {
        int result;
        memcpy(&copy_read_fds_set, &read_fds_set, sizeof(read_fds_set));
        if ((result = select(nfds + 1, &copy_read_fds_set, NULL, NULL, NULL)) > 0)
        {
            for (int i = 0; i < CANTCHILD && argc; i++,argv += 1, argc -= 1)
            {
                if (FD_ISSET(read_fds[i], &copy_read_fds_set))
                {
                    char buffer[1024];
                    int n = read(read_fds[i], buffer, strlen(buffer));
                    printf("%s\n", buffer);
                    memset (buffer, 0, sizeof (buffer));
                    write(write_fds[i], *argv, strlen(*argv));
                    write(write_fds[i], "\n", 1);
                }
            }
        }
        else
        {
            perror("select");
            break;
        }
    }
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

void redirect_fd(int src_fd, int dest_fd, int fd_close){
    if(close(fd_close) == -1){
        exit_failure("close unused fd");
    }
    safe_dup2(src_fd, dest_fd);
    if(close(src_fd) == -1){
        exit_failure("close");
    }
}