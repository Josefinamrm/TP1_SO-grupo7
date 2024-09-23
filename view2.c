// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com



#include "utils.h"

#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>

#define SHM_NAME_SIZE  256

int main(int argc, char * argv[]){

    char buffer[BUFFER_LENGTH];
    char toRead;
    int counter = 0;

    ssize_t count;

    char * fifo_name = "/view_pipe";
    int fifo_fd = open(fifo_name, O_RDONLY);

    char * check_view_name = "/check_view";
    sem_t * check_view_sem = safe_sem_open(check_view_name);
    sem_post(check_view_sem);


    while ((count = read(fifo_fd, &toRead, 1)) > 0){
        
        if (toRead == '\n'){
            buffer[counter] = '\0';
            counter = 0;

            write(STDOUT_FILENO, buffer, strlen(buffer));
            write(STDOUT_FILENO, "\n", 1);
            fflush(stdout);
        }
        else{
            buffer[counter++] = toRead;
        }
    }
    
    safe_sem_close(check_view_sem);
    sem_unlink(check_view_name);

    exit(EXIT_SUCCESS);
}
