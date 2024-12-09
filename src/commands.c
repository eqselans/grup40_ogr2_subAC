#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include "commands.h"

void execute_command(const char *command, char *args[]) {
    pid_t pid = fork();
    if (pid == 0) {
        execvp(command, args);
        perror("execvp failed");
        exit(EXIT_FAILURE);
    } else if (pid > 0) {
        int status;
        waitpid(pid, &status, 0);
        printf("Command executed with status: %d\n", WEXITSTATUS(status));
    } else {
        perror("fork failed");
    }
}
