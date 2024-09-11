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


int main(int argc, char * argv[]){ // en el argv[1] tengo el nombre de la shm

    int fd;
    char * addr;
    struct stat st;

    sems * sem;

    
    // int down = sem_wait(&sem->mutex);
    // if(down == -1){
    //     perror("sem_wait");
    //     return 1;
    // }    

    fd = shm_open(argv[1], O_RDONLY, 0);
    if(fd == -1){
        perror("shm_open");
        return 1;
    }

    if(fstat(fd, &st) == -1){
        perror("fstat");
        return 1;
    }

    addr = mmap(NULL, st.st_size, PROT_READ, MAP_SHARED, fd, 0);
    if(addr == MAP_FAILED){
        perror("mmap");
        return 1;
    }

    char * ptr = addr; // en addr tengo la direcc de memoria de la shm

    for(int i = 0; i < argc; i++){ // CAMBIAR 

        write(STDOUT_FILENO, ptr, strlen(ptr)); // imprimo el contenido de la shm
        printf("\n");
        ptr += (strlen(ptr)+ 1); // avanzo
    }

    munmap(addr, st.st_size);

    // int up = sem_post(&sem->mutex);
    // if(up == -1){
    //     perror("sem_post");
    //     return 1;
    // }

    close(fd);

    return 0;
}

/* EL CDT DE PRÁCTICA 

READ:
    sem_wait
    sem_wait
    algortimo read
    sem_post

OPEN:   
    sem_open
    sem_open
    shm_open
    mmap

CLOSE:
    unmap
    sem_close
    close
    free

*/