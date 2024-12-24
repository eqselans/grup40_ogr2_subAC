/*
2.Öğretim A Grubu Grup 40

Recep Salih Kılınç
Emirhan Aksu
Semih Özçaka
Yusuf Talha Aktürk
Ömer Batuhan Yıldız
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "parser.h"

/* 
 * Yardımcı fonksiyon
 */
static void add_command(command_sequence_t *seq, char *cmd) {
    seq->commands[seq->count] = strdup(cmd);
    seq->count++;
}

command_sequence_t* parse_semicolon(char *line) {
    // command_sequence_t ayır
    command_sequence_t *seq = malloc(sizeof(command_sequence_t));
    if (!seq) return NULL;
    seq->commands = malloc(sizeof(char*) * 128);
    seq->count = 0;

    char *token = strtok(line, ";");
    while (token) {
        // Kenarlardaki boşlukları temizle
        while (isspace((unsigned char)*token)) token++;
        char *end = token + strlen(token) - 1;
        while (end > token && isspace((unsigned char)*end)) {
            *end = '\0';
            end--;
        }
        add_command(seq, token);
        token = strtok(NULL, ";");
    }
    return seq;
}

void free_command_sequence(command_sequence_t *cmd_seq) {
    if (!cmd_seq) return;
    for (int i = 0; i < cmd_seq->count; i++) {
        free(cmd_seq->commands[i]);
    }
    free(cmd_seq->commands);
    free(cmd_seq);
}

/* Pipe'ları parse etme */
static pipeline_t* create_pipeline(int n) {
    pipeline_t *pline = malloc(sizeof(pipeline_t));
    pline->commands = calloc(n, sizeof(command_t));
    pline->count = n;
    return pline;
}

/* String içindeki argümanları (boşluk) ayırıp command_t'ye doldurur */
static void parse_args(command_t *cmdinfo, char *str) {
    cmdinfo->cmd = NULL;
    cmdinfo->infile = NULL;
    cmdinfo->outfile = NULL;
    cmdinfo->background = 0;
    for (int i = 0; i < MAX_ARGS; i++) {
        cmdinfo->args[i] = NULL;
    }

    // Tokenize (boşluğa göre)
    char *token = strtok(str, " \t");
    int idx = 0;

    while (token) {
        if (strcmp(token, "<") == 0) {
            // Girdi dosyası
            token = strtok(NULL, " \t");
            if (token) {
                cmdinfo->infile = strdup(token);
            }
        } else if (strcmp(token, ">") == 0) {
            // Çıkış dosyası
            token = strtok(NULL, " \t");
            if (token) {
                cmdinfo->outfile = strdup(token);
            }
        } else if (strcmp(token, "&") == 0) {
            // Arka plan bayrağı
            cmdinfo->background = 1;
        } else {
            // Normal argüman
            cmdinfo->args[idx] = strdup(token);
            idx++;
        }
        token = strtok(NULL, " \t");
    }

    if (cmdinfo->args[0]) {
        cmdinfo->cmd = cmdinfo->args[0];
    }
}

pipeline_t* parse_pipeline(char *line) {
    // Boru sayısı kaçsa, o kadar komut var
    int count = 1;
    for (int i = 0; i < (int)strlen(line); i++) {
        if (line[i] == '|') count++;
    }

    pipeline_t *pline = create_pipeline(count);
    if (!pline) return NULL;

    // pipe'ları ayır
    char *temp = strdup(line);
    char *saveptr;
    char *token = strtok_r(temp, "|", &saveptr);
    int idx = 0;
    while (token) {
        // Kenarlardaki boşlukları sil
        while (isspace((unsigned char)*token)) token++;
        char *end = token + strlen(token) - 1;
        while (end > token && isspace((unsigned char)*end)) {
            *end = '\0';
            end--;
        }

        parse_args(&pline->commands[idx], token);
        idx++;

        token = strtok_r(NULL, "|", &saveptr);
    }
    free(temp);

    return pline;
}

void free_pipeline(pipeline_t *pline) {
    if (!pline) return;
    for (int i = 0; i < pline->count; i++) {
        for (int j = 0; j < MAX_ARGS; j++) {
            if (pline->commands[i].args[j]) {
                free(pline->commands[i].args[j]);
            }
        }
        if (pline->commands[i].infile) free(pline->commands[i].infile);
        if (pline->commands[i].outfile) free(pline->commands[i].outfile);
    }
    free(pline->commands);
    free(pline);
}
