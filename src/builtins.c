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
#include <sys/wait.h>
#include <string.h>
#include <fcntl.h>
#include "parser.h"

extern void add_bg_job(pid_t pid);

int is_quit_command(pipeline_t *pline) {
    if (pline->count == 1 && pline->commands[0].cmd
        && strcmp(pline->commands[0].cmd, "quit") == 0) {
        return 1;
    }
    return 0;
}

int background_jobs_active(void *bg_jobs_ptr, int size);

void execute_single_command(command_t *cmd, void *bg_jobs_ptr __attribute__((unused)), int size __attribute__((unused))) {
    if (cmd->cmd == NULL) {
        return; 
    }

    pid_t pid = fork();
    if (pid < 0) {
        perror("fork hatası");
        return;
    } else if (pid == 0) {
        if (cmd->infile) {
            int fd_in = open(cmd->infile, O_RDONLY);
            if (fd_in < 0) {
                fprintf(stderr, "%s giriş dosyası bulunamadı.\n", cmd->infile);
                fflush(stderr);
                exit(1);
            }
            dup2(fd_in, STDIN_FILENO);
            close(fd_in);
        }

        // Çıkış yönlendirme
        if (cmd->outfile) {
            int fd_out = open(cmd->outfile, O_WRONLY | O_CREAT | O_TRUNC, 0666);
            if (fd_out < 0) {
                perror("open outfile");
                exit(1);
            }
            dup2(fd_out, STDOUT_FILENO);
            close(fd_out);
        }

        // Exec
        execvp(cmd->cmd, cmd->args);
        perror("execvp");
        exit(1);
    } else {
        // Ebeveyn
        if (cmd->background) {
            // Arka plan
            add_bg_job(pid);
        } else {
            // Ön plan
            int status;
            waitpid(pid, &status, 0);
        }
    }
}

/*
 * Boru içeren komutları (pipeline) çalıştırma.
 */
void execute_pipeline(pipeline_t *pline, void *bg_jobs_ptr __attribute__((unused)), int size __attribute__((unused))) {
    int n = pline->count;
    int pipes[2 * (n - 1)]; // Her komut arasına 2 fd

    for (int i = 0; i < n - 1; i++) {
        if (pipe(pipes + i * 2) < 0) {
            perror("pipe");
            return;
        }
    }

    // Her komut için fork
    pid_t pids[n];

    for (int i = 0; i < n; i++) {
        pid_t pid = fork();
        if (pid < 0) {
            perror("fork");
            return;
        } else if (pid == 0) {
            // Çocuk
            // Giriş yönlendirme
            if (pline->commands[i].infile) {
                int fd_in = open(pline->commands[i].infile, O_RDONLY);
                if (fd_in < 0) {
                    fprintf(stderr, "%s giriş dosyası bulunamadı.\n", pline->commands[i].infile);
                    fflush(stderr);
                    exit(1);
                }
                dup2(fd_in, STDIN_FILENO);
                close(fd_in);
            }
            // Eğer ilk komut değilse, öncesindeki pipe'tan oku
            if (i > 0) {
                dup2(pipes[(i - 1) * 2], STDIN_FILENO);
            }
            // Son komut değilse, bir sonraki pipe'a yaz
            if (i < n - 1) {
                dup2(pipes[i * 2 + 1], STDOUT_FILENO);
            }

            // Tüm pipe fd'lerini kapat
            for (int j = 0; j < 2 * (n - 1); j++) {
                close(pipes[j]);
            }

            // Çıkış yönlendirme
            if (pline->commands[i].outfile) {
                int fd_out = open(pline->commands[i].outfile, O_WRONLY | O_CREAT | O_TRUNC, 0666);
                if (fd_out < 0) {
                    perror("open outfile");
                    exit(1);
                }
                dup2(fd_out, STDOUT_FILENO);
                close(fd_out);
            }

            execvp(pline->commands[i].cmd, pline->commands[i].args);
            perror("execvp");
            exit(1);
        } else {
            // Ebeveyn
            pids[i] = pid;
        }
    }

    // Ebeveyn, pipe fd'lerini kapatsın
    for (int j = 0; j < 2 * (n - 1); j++) {
        close(pipes[j]);
    }

    // Arka plan mı?
    if (pline->commands[n - 1].background) {
        for (int i = 0; i < n; i++) {
            add_bg_job(pids[i]);
        }
    } else {
        // Ön planda: Tümünün bitmesini bekle
        for (int i = 0; i < n; i++) {
            int status;
            waitpid(pids[i], &status, 0);
        }
    }
}

/*
 * Arka plan işlemi aktif mi?
 */
int background_jobs_active(void *bg_jobs_ptr, int size) {
    typedef struct {
        pid_t pid;
        int active;
    } bg_job_t;

    bg_job_t *jobs = (bg_job_t *)bg_jobs_ptr;

    for (int i = 0; i < size; i++) {
        if (jobs[i].active == 1) {
            return 1;
        }
    }
    return 0;
}
