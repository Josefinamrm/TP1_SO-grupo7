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
#include <select.h>

#define CANTCHILD 5

// Function that exits with code EXIT_FAILURE and displays error message
void exit_failure(char *message);

// Function that forks safely. Returns child fd if succesful, otherwise it ends execution of the program
int safe_fork();

// Function that dups safely
void safe_dup2(int src_fd, int dest_fd);

// Function that copies one array to another, of the same dimension and of the same type
void copy_array(int * src, int * dest, int dim);

int main(int argc, char *argv[])
{

    // lo pongo porlas, capaz después no lo necesito
    int nfds;

    for (int i = 0; i < CANTCHILD; i++)
    {
        // Create pipes, one for reading and one for writing
        int r_pipe[2];
        int w_pipe[2];

        if (pipe(r_pipe) == -1 || pipe(w_pipe) == -1)
            exit_failure("pipe");

        int pid = safe_fork();
        if (pid == 0)
        {
            // cierro los extremos de los pipes que no voy a usar
            close(r_pipe[0]);
            close(w_pipe[1]);
            // redirijo stdin y stdout a los pipes
            safe_dup2(r_pipe[1], STDOUT_FILENO);
            safe_dup2(w_pipe[0], STDIN_FILENO);
            // cierro los extremos de los pipes ya duplicados
            close(r_pipe[1]);
            close(w_pipe[0]);
            // redirijo stderr ?

            // ahora debería de tener
            // 0 -> read end del w_pipe
            // 1 -> write end del r_pipe
            // 2-> tty

            // acá tengo duda de si poner el path así
            char *child_argv[] = {"./slave", NULL};
            execve("./slave", child_argv, NULL);
            exit_failure("execve\n");
        }
        close(r_pipe[1]);
        close(w_pipe[0]);
        read_fds[i] = r_pipe[0];
        // ahora debería de tener
            // 0 -> tty
            // 1 -> tty
            // 2-> tty
            // 3 -> lectura
            // 6 -> escritura
        // acá le mandaría los dos archivos por ejemplo con dos write al w_pipe[1] y decremento argc en 2 
    }
    nfds = read_fds[CANTCHILD - 1] + 1;

    // mientras hayan archivos por procesar (argc > 0)
    // select(nfds, copy_read_fds, NULL, NULL, NULL, NULL) = 2 
    int copy_read_fds[CANTCHILD]; 
    while(argc > 0){
        copy_array(read_fds, copy_read_fds, CANTCHILD);

    }

    // 3 -> write(6)

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

void copy_array(int * src, int * dest, int dim){
    for(int i = 0; i < dim; i++){
        dest[i] = src[i];
    }
}
