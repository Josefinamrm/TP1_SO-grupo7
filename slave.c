// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com

#define _GNU_SOURCE
#include "utils.h"

int main(int argc, char *argv[]){

    char buffer[BUFFER_SIZE];
    char toRead;
    int bytesRead;
    int counter = 0;
    char command_shell[BUFFER_SIZE];
    char output[BUFFER_SIZE];

    ssize_t count;
    while ((count = read(STDIN_FILENO, &toRead, 1)) > 0){
        
        if (toRead == '\n'){
            buffer[counter] = '\0';
            counter = 0;

            snprintf(command_shell, BUFFER_SIZE, "md5sum -z \"%s\"", buffer);

            FILE *md5 = safe_popen(command_shell, "r");
            char *cmd = safe_fgets(command_shell, BUFFER_SIZE, md5);
            pid_t pid = safe_getpid();
            snprintf(output, BUFFER_SIZE, "%s\t-\t%d\n", cmd, pid);

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