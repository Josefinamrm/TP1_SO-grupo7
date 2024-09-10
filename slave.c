/*

- debe recibir el/los paths de los archivos a procesar y debe iniciar el programa correspondiente para procesarlos (md5sum)
- debe enviar la información relevante del procesamiento al proceso aplicación
- debe recibir el output de md5sum utilizadno alfun mecanismo de IPC más sofisticado

*/
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

#define BUF_LENGTH 1024

int main(void)
{
    char buffer[BUF_LENGTH];
    char toRead;
    int bytesRead;
    int counter = 0;

    while((bytesRead = read(STDIN_FILENO, &toRead, 1)) > 0 && counter < BUF_LENGTH){
        if(toRead == '\0' || counter == BUF_LENGTH-1){
            buffer[counter] = '\0';
            write(STDOUT_FILENO, buffer, strlen(buffer)+1);
            fflush(stdout);
            counter=0;
        }else{
            buffer[counter++] = toRead;
        }
    }

    exit(EXIT_SUCCESS);
}


