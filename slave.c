/*

- debe recibir el/los paths de los archivos a procesar y debe iniciar el programa correspondiente para procesarlos (md5sum)
- debe enviar la información relevante del procesamiento al proceso aplicación
- debe recibir el output de md5sum utilizadno alfun mecanismo de IPC más sofisticado

*/
#define _GNU_SOURCE
#include "utils.h"
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

#define BUFFER_SIZE 1024
#define COMMAND_SIZE 2048

int main(int argc, char *argv[]){

    char buffer[BUFFER_SIZE];
    char toRead;
    int bytesRead;
    int counter = 0;
    char command_shell[COMMAND_SIZE];
    char output[COMMAND_SIZE];

    ssize_t count;
    while ((count = read(STDIN_FILENO, &toRead, 1)) > 0){
        
        if (toRead == '\n'){
            buffer[counter] = '\0';
            counter = 0;

            snprintf(command_shell, COMMAND_SIZE, "md5sum -z \"%s\"", buffer);

            FILE *md5 = safe_popen(command_shell, "r");
            char *cmd = safe_fgets(command_shell, COMMAND_SIZE, md5);
            pid_t pid = safe_getpid();
            snprintf(output, COMMAND_SIZE, "%s\t-\t%d\n", cmd, pid);

            pclose(md5);

            write(STDOUT_FILENO, output, strlen(output));
            fflush(stdout);
        }
        else{
            buffer[counter++] = toRead;
        }
    }

    return 0;
}