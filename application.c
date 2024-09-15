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
#define BUFFER_LENGTH 1024
#define SHM_LENGTH 1024

int main(int argc, char *argv[]){

    if (argc < 2){
        printf("Program must be called with at least one file to analyse\n");
        exit(EXIT_FAILURE);
    }


    // Read and Write file descriptors
    nfds_t open_read_fds;
    struct pollfd *readable_fds = calloc(CANTSLAVES, sizeof(struct pollfd));
    if (readable_fds == NULL){
        exit_failure("calloc");
    }
    int writeable_fds[CANTSLAVES];


    // Shared memory
    char shm_name[SHM_LENGTH] = "/shm";
    snprintf((char *)shm_name, sizeof(shm_name) - 1, "/shm%d", getpid());
    char *shm_ptr = initialize_shm(shm_name, SHM_LENGTH);

    write(STDOUT_FILENO, shm_name, strlen(shm_name));
    fflush(stdout);


    // Semaphore for synchronization between view and application
    char *can_read_name = "can_read";
    sem_t *can_read_sem = sem_open(can_read_name, O_CREAT, 0644, 0);


    // Semaphore to check if view process is running
    char * check_view_name = "check_view";
    sem_t * check_view_sem = sem_open(check_view_name, O_CREAT, 0644, 0);


    int view_running = 1, s;
    struct timespec ts;
    clock_gettime(CLOCK_REALTIME, &ts);
    ts.tv_sec += TIME;


    if((s = sem_timedwait(check_view_sem, &ts)) == -1){
        if(errno == ETIMEDOUT){
            view_running = 0;
        }
    }


    // Initialize slaves and distribute files
    argc--;
    argv++;
    open_read_fds = ininitalize_slaves(readable_fds, writeable_fds, argv, &argc);


    // Variables for output file
    FILE *output_file;
    output_file = fopen("respuesta.txt", "w");
    if (output_file == NULL){
        exit_failure("fopen");
    }


    // Read results
    int result;
    while (open_read_fds > 0){
        result = poll(readable_fds, CANTSLAVES, -1);

        if (result == -1){
            exit_failure("poll");
        }

        for (int i = 0; i < CANTSLAVES; i++){
            if (readable_fds[i].revents != 0){
                // Data available
                if (readable_fds[i].revents & POLLIN){
                    char to_read;
                    int n, read_flag = 0, counter = 0;
                    char buffer[BUFFER_LENGTH];
                    
                    while (!read_flag && (n = read(readable_fds[i].fd, &to_read, 1)) > 0){
                        if (to_read == '\n'){
                            buffer[counter] = '\0';

                            if(view_running){
                                write_to_shm(shm_ptr, buffer, counter + 1, can_read_sem);
                                shm_ptr += counter + 1;
                            }

                            fprintf(output_file, "%s\n", buffer);
                            counter = 0;
                            read_flag = 1;

                            if (argc > 0){
                                write_to_fd(writeable_fds[i], *argv);
                                argv++;
                                argc--;
                            }
                            else{
                                if (writeable_fds[i] != -1){
                                    safe_close(writeable_fds[i]);
                                    writeable_fds[i] = -1;
                                }
                            }
                        }
                        else{
                            buffer[counter++] = to_read;
                        }
                    }
                }
                // Error or EOF
                else{
                    safe_close(readable_fds[i].fd);
                    readable_fds[i].fd = -1;
                    open_read_fds--;
                }
            }
        }
    }

    write_to_shm(shm_ptr, TERMINATION, strlen(TERMINATION) + 1, can_read_sem);

    munmap(shm_ptr, SHM_LENGTH);
    shm_unlink(shm_name);
    sem_close(can_read_sem);
    sem_unlink(can_read_name);
    fclose(output_file);
    free(readable_fds);

    exit(EXIT_SUCCESS);
}