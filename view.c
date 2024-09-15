/*

- debe recibir por entrada esrpandar y como parámmetro la información decesaria para conectarse al buffer compartido
- debe mostrar en panrall ael contenido del buffer de llegada a medida que se va cargansoo el mismo. El buffer debe tener:
                * nombre del archivo
                * Md5 del archivo
                * ID del esclavo que lo procesó

*/


#include "utils.h"

#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>

#define SHM_NAME_SIZE  256
#define BUFFER_LENGTH 1024

int main(int argc, char * argv[]){

    int shm_fd;
    char * addr;
    struct stat copy;


    char shm_name[SHM_NAME_SIZE]={0};
    // si está en la línea de comandos, el primer argumento 
    if(argc > 1){
        strncpy(shm_name, argv[1], sizeof(shm_name)-1);
        shm_name[strlen(shm_name)] = '\0';
    }
    else{
        // sino, por stdin (pipe)
        size_t count =read(STDIN_FILENO, shm_name, SHM_NAME_SIZE);
    }


    shm_fd = shm_open(shm_name, O_RDONLY, 0);
    if(shm_fd == -1){
        perror("shm_open");
        return 1;
    }

    if(fstat(shm_fd, &copy) == -1){
        perror("fstat");
        return 1;
    }

    addr = mmap(NULL, copy.st_size, PROT_READ, MAP_SHARED, shm_fd, 0);
    if(addr == MAP_FAILED){
        perror("mmap");
        return 1;
    }

    sem_t * can_read = sem_open("can_read", O_CREAT);

    char * ptr = addr;

    while(strcmp(ptr, TERMINATION) != 0){
        sem_wait(can_read);
        write_to_fd(STDOUT_FILENO, ptr);
        ptr += strlen(ptr)+1;
        fflush(stdout);
    }



    munmap(addr, copy.st_size);
    close(shm_fd);
    sem_close(can_read);

    exit(EXIT_SUCCESS);
}
