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
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <signal.h>
#include <fcntl.h>

#include "shell.h"
#include "parser.h"
#include "builtins.h"

#define MAX_BG_JOBS 128

typedef struct {
    pid_t pid;
    int active;
} bg_job_t;

static bg_job_t bg_jobs[MAX_BG_JOBS];

/* Arka plan işleminin sonlanmasını yakalamak için handler */
void sigchld_handler(int sig) {
    (void)sig; // Unused param
    int status;
    pid_t pid;

    // Non-blocking şekilde bekle
    while ((pid = waitpid(-1, &status, WNOHANG)) > 0) {
        // Arka plan işlemi listesinde bul ve bilgiyi bas
        for (int i = 0; i < MAX_BG_JOBS; i++) {
            if (bg_jobs[i].pid == pid && bg_jobs[i].active == 1) {
                bg_jobs[i].active = 0;
                printf("[%d] retval: %d\n", pid, WEXITSTATUS(status));
                fflush(stdout);
                break;
            }
        }
    }
}

/* Arka plan job listesini başlatan fonksiyon */
void init_bg_jobs() {
    for (int i = 0; i < MAX_BG_JOBS; i++) {
        bg_jobs[i].pid = 0;
        bg_jobs[i].active = 0;
    }
}

/* Arka plan job listesine yeni ekle */
void add_bg_job(pid_t pid) {
    for (int i = 0; i < MAX_BG_JOBS; i++) {
        if (bg_jobs[i].active == 0) {
            bg_jobs[i].active = 1;
            bg_jobs[i].pid = pid;
            return;
        }
    }
    fprintf(stderr, "Arka plan işlemi listesi dolu!\n");
}

/* Tüm arka plan işlemleri bitene kadar bekler */
void wait_all_bg_jobs() {
    int still_active = 1;
    while (still_active) {
        still_active = 0;
        for (int i = 0; i < MAX_BG_JOBS; i++) {
            if (bg_jobs[i].active == 1) {
                still_active = 1;
                break;
            }
        }
        // Aktif arka plan işlemleri bitene kadar bekle
        if (still_active) {
            usleep(100000); // 0.1 sn bekle
        }
    }
}

/* Shell döngüsü */
void start_shell() {
    char line[1024];
    struct sigaction sa;
    sa.sa_handler = sigchld_handler;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = SA_RESTART;
    sigaction(SIGCHLD, &sa, NULL);

    init_bg_jobs();

    while (1) {
        printf("> ");
        fflush(stdout);

        /* Komut satırını oku */
        if (fgets(line, sizeof(line), stdin) == NULL) {
            // Kullanıcı EOF (Ctrl+D) girdi veya hata oluştu
            printf("\n");
            break;
        }

        // Çizgisonu karakterini temizle
        line[strcspn(line, "\n")] = '\0';

        // Komut boşsa devam
        if (strlen(line) == 0) {
            continue;
        }

        // Satırı noktalarına (;) göre parçala
        command_sequence_t *cmd_seq = parse_semicolon(line);
        if (!cmd_seq) {
            // Hatalı parse
            continue;
        }

        // Her bir komutu sırayla çalıştır
        for (int i = 0; i < cmd_seq->count; i++) {
            // Parse pipe
            pipeline_t *pline = parse_pipeline(cmd_seq->commands[i]);
            if (!pline) {
                continue;
            }

            // quit komutunu yakala (builtin)
            if (is_quit_command(pline)) {
                if (background_jobs_active(bg_jobs, MAX_BG_JOBS)) {
                    wait_all_bg_jobs();
                }
                free_pipeline(pline);
                free_command_sequence(cmd_seq);
                return;
            }

            // Boru (pipe) işlemi
            // Birden fazla komut varsa pipeline oluşturup çalıştır.
            if (pline->count > 1) {
                execute_pipeline(pline, bg_jobs, MAX_BG_JOBS);
            } else {
                // Tek komut var
                execute_single_command(&pline->commands[0], bg_jobs, MAX_BG_JOBS);
            }

            free_pipeline(pline);
        }

        free_command_sequence(cmd_seq);
    }

    // Kabuktan çıkmadan önce arka plan işlerini bekle
    wait_all_bg_jobs();
}
