/*
2.Öğretim A Grubu Grup 40

Recep Salih Kılınç
Emirhan Aksu
Semih Özçaka
Yusuf Talha Aktürk
Ömer Batuhan Yıldız
*/

#ifndef PARSER_H
#define PARSER_H

#define MAX_ARGS 64

typedef struct {
    char *cmd;
    char *args[MAX_ARGS];
    char *infile;
    char *outfile;
    int background;
} command_t;

typedef struct {
    command_t *commands;    
    int count;
} pipeline_t;

typedef struct {
    char **commands;
    int count;
} command_sequence_t;

/* Noktalı virgülleri (;) ayırmak için */
command_sequence_t* parse_semicolon(char *line);
void free_command_sequence(command_sequence_t *cmd_seq);

/* pipe (|) ayırmak için */
pipeline_t* parse_pipeline(char *line);
void free_pipeline(pipeline_t *pline);

#endif
