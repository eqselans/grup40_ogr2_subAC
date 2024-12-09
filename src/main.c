#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <stdlib.h>
#include <unistd.h>
#include "commands.h"
#include "redirection.h"
#include "background.h"
#include "pipe.h"

#define MAX_INPUT 1024

void display_prompt() {
    printf("> ");
    fflush(stdout);
}

int main() {
    char input[MAX_INPUT];
    char *args[50];

    while (true) {
        display_prompt();

        // Kullanıcı girdisi al
        if (fgets(input, MAX_INPUT, stdin) == NULL) {
            perror("Input error");
            break;
        }

        // Girdiyi temizle
        input[strcspn(input, "\n")] = '\0';

        // Quit komutunu kontrol et
        if (strcmp(input, "quit") == 0) {
            printf("Quitting shell...\n");
            break;
        }

        // Komutları ayrıştır
        char *token = strtok(input, " ");
        int i = 0;
        while (token != NULL) {
            args[i++] = token;
            token = strtok(NULL, " ");
        }
        args[i] = NULL;

        // Arka plan komut kontrolü
        if (i > 0 && strcmp(args[i - 1], "&") == 0) {
            args[i - 1] = NULL;
            execute_in_background(args[0], args);
        } else {
            execute_command(args[0], args);
        }
    }

    return 0;
}
