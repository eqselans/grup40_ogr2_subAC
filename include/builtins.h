/*
2.Öğretim A Grubu Grup 40

Recep Salih Kılınç
Emirhan Aksu
Semih Özçaka
Yusuf Talha Aktürk
Ömer Batuhan Yıldız
*/

#ifndef BUILTINS_H
#define BUILTINS_H

#include "parser.h"

void execute_single_command(command_t *cmd, void *bg_jobs_ptr, int size);
void execute_pipeline(pipeline_t *pline, void *bg_jobs_ptr, int size);
int is_quit_command(pipeline_t *pline);
int background_jobs_active(void *bg_jobs_ptr, int size);

#endif