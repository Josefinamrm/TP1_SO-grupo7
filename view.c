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
    struct stat copy;

    semaphore * sem;

    

    fd = shm_open(argv[1], O_RDONLY, 0);
    if(fd == -1){
        perror("shm_open");
        return 1;
    }

    if(fstat(fd, &copy) == -1){
        perror("fstat");
        return 1;
    }

    addr = mmap(NULL, copy.st_size, PROT_READ, MAP_SHARED, fd, 0);
    if(addr == MAP_FAILED){
        perror("mmap");
        return 1;
    }

    char * ptr = addr; // en addr tengo la direcc de memoria de la shm

    while(sem_wait(&sem->r)){ // CAMBIAR 

        int down = sem_wait(&sem->mutex);
        if(down == -1){
            perror("sem_wait");
            return 1;
        }   
        write(STDOUT_FILENO, ptr, strlen(ptr)); // imprimo el contenido de la shm
        int up = sem_post(&sem->mutex);
        if(up == -1){
            perror("sem_wait");
            return 1;
        }
  
        ptr += (strlen(ptr)+ 1); // avanzo
    }

    munmap(addr, copy.st_size);


    close(fd);


    return 0;
}
