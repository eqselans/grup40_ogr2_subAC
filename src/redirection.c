#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include "redirection.h"

void redirect_input(const char *file) {
    int fd = open(file, O_RDONLY);
    if (fd < 0) {
        perror("Input file not found");
        return;
    }
    dup2(fd, STDIN_FILENO);
    close(fd);
}

void redirect_output(const char *file) {
    int fd = open(file, O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (fd < 0) {
        perror("Failed to open output file");
        return;
    }
    dup2(fd, STDOUT_FILENO);
    close(fd);
}
