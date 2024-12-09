#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "background.h"

void execute_in_background(const char *command, char *args[]) {
    pid_t pid = fork();
    if (pid == 0) {
        execvp(command, args);
        perror("execvp failed");
        exit(EXIT_FAILURE);
    } else if (pid > 0) {
        printf("[pid %d] Background process started\n", pid);
    } else {
        perror("fork failed");
    }
}
