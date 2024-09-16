// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com



#include "utils.h"

#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>

#define SHM_NAME_SIZE  256

int main(int argc, char * argv[]){

    int shm_fd;
    int data_available = 1;
    char * addr;

    char shm_name[SHM_NAME_SIZE]={0};
    // si está en la línea de comandos, el primer argumento 
    if(argc > 1){
        strncpy(shm_name, argv[1], sizeof(shm_name)-1);
    }
    else{
        // sino, por stdin (pipe)
        read(STDIN_FILENO, shm_name, SHM_NAME_SIZE);
    }

    addr = open_shm_object(shm_name, O_RDONLY, PROT_READ);

    char * check_view_name = "/check_view";
    sem_t * check_view_sem = safe_sem_open(check_view_name);
    sem_post(check_view_sem);

    char * can_read_name = "/can_read";
    sem_t * can_read_sem = safe_sem_open(can_read_name);

    char * ptr = addr;


    while(data_available){

        sem_wait(can_read_sem);

        if(strcmp(ptr, TERMINATION) == 0){
            data_available = 0;
        }
        else{
            write_to_fd(STDOUT_FILENO, ptr);
            ptr += strlen(ptr)+1;
            fflush(stdout);
        }
    }
    
    safe_sem_close(can_read_sem);
    sem_unlink(can_read_name);
    safe_sem_close(check_view_sem);
    sem_unlink(check_view_name);

    exit(EXIT_SUCCESS);
}
