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

    int shm_fd;
    char * addr;
    struct stat copy;

    shm_fd = shm_open(argv[1], O_RDONLY, 0);
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

    char * ptr = addr; // en addr tengo la direcc de memoria de la shm

    for(int i = 0; i < copy.st_size; i++){
        write(STDOUT_FILENO, ptr, strlen(ptr)); // imprimo el contenido de la shm
        ptr += (strlen(ptr)+ 1); // avanzo
    }

    munmap(addr, copy.st_size);
    shm_unlink(argv[1]);
    close(shm_fd);


    return 0;
}
