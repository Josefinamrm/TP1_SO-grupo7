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
#include "utils.h"
#define CANTSLAVES 1
#define INITIAL_FILES 2
#define BUFFER_LENGTH 1024
#define SHM_LENGTH 1024

int main(int argc, char *argv[])
{

    if (argc < 2)
    {
        printf("Program must be called with at least one file to analyse\n");
        exit(EXIT_FAILURE);
    }

    // Variables for file descriptors, used in IPC
    nfds_t open_read_fds = 0;
    struct pollfd *readable_fds = calloc(CANTSLAVES, sizeof(struct pollfd));
    if (readable_fds == NULL)
    {
        exit_failure("calloc");
    }
    int write_fds[CANTSLAVES];

    // Variables for shared memory, if the view process appears
    char shm_name[SHM_LENGTH] = "/shm";
    snprintf((char *)shm_name, sizeof(shm_name) - 1, "/shm%d", getpid());

    int shm_fd = shm_open(shm_name, O_CREAT | O_RDWR, 0);
    if (shm_fd == -1)
    {
        exit_failure("shm_open");
    }
    if (ftruncate(shm_fd, SHM_LENGTH) == -1)
    {
        exit_failure("ftruncate");
    }
    char *shm_ptr = mmap(NULL, SHM_LENGTH, PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);
    if (shm_ptr == MAP_FAILED)
    {
        exit_failure("mmap");
    }

    // Semaphore for synchronization between view and application
    char *sem_name = "/can_read";
    sem_t *can_read = sem_open(sem_name, O_CREAT, 0644, 0);

    write(STDOUT_FILENO, shm_name, strlen(shm_name));
    fflush(stdout);
    sleep(5);

    // Make cm arguments ready for passing
    argc--;
    argv++;

    for (int i = 0; i < CANTSLAVES; i++)
    {
        // Create pipes, one for reading and one for writing
        int c2p_pipe[2];
        int p2c_pipe[2];

        safe_pipe(c2p_pipe);
        safe_pipe(p2c_pipe);

        int pid = safe_fork();
        if (pid == 0)
        {
            for (int i = 0; i < open_read_fds; i++)
            {
                safe_close(readable_fds[i].fd);
                safe_close(write_fds[i]);
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
        write_fds[i] = p2c_pipe[1];

        for (int i = 0; i < INITIAL_FILES && i <= argc; i++, argc--, argv++)
        {
            write_to_fd(p2c_pipe[1], *argv);
        }
    }

    // Variables for output file
    FILE *rta_ptr;
    rta_ptr = fopen("respuesta.txt", "w");
    if (rta_ptr == NULL)
    {
        exit_failure("fopen");
    }

    // Read results
    int result;
    while (open_read_fds > 0)
    {
        result = poll(readable_fds, CANTSLAVES, -1);
        if (result == -1)
        {
            exit_failure("poll");
        }

        for (int i = 0; i < CANTSLAVES; i++)
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
                        if (to_read == '\n')
                        {
                            buffer[counter] = '\0';
                            memcpy(shm_ptr, buffer, counter + 1);
                            sem_post(can_read);
                            shm_ptr += counter + 1;

                            fprintf(rta_ptr, "%s\n", buffer);
                            counter = 0;
                            read_flag = 1;
                            if (argc > 0)
                            {
                                write_to_fd(write_fds[i], *argv);
                                argv++;
                                argc--;
                            }
                            else
                            {
                                if (write_fds[i] != -1)
                                {
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
                // Error or EOF
                else
                {
                    safe_close(readable_fds[i].fd);
                    readable_fds[i].fd = -1;
                    open_read_fds--;
                }
            }
        }
    }

    memcpy(shm_ptr, TERMINATION, strlen(TERMINATION) + 1);
    sem_post(can_read);

    munmap(shm_ptr, SHM_LENGTH);
    close(shm_fd);
    shm_unlink(shm_name);
    sem_close(can_read);
    sem_unlink(sem_name);
    fclose(rta_ptr);

    exit(EXIT_SUCCESS);
}
