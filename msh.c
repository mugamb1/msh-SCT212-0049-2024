#define _POSIX_C_SOURCE 200809L
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <fcntl.h>


#define MAX_ARGS 128

int parse_line(char *line, char **argv) {
    int argc = 0;
    char *tok = strtok(line, " \t\r\n");
    while (tok != NULL && argc < MAX_ARGS - 1) {
        argv[argc++] = tok;
        tok = strtok(NULL, " \t\r\n");
    }
    argv[argc] = NULL;
    return argc;
}

int main(void) {
    char *line = NULL;
    size_t cap = 0;
    char *argv[MAX_ARGS];
    int last_status = 0;

    while (1) {
        int bg_status;
        pid_t done_pid;
        while ((done_pid = waitpid(-1, &bg_status, WNOHANG)) > 0) {
            printf("[bg] PID %d finished\n", done_pid);
        }

        printf("msh[%d]> ", last_status);
        fflush(stdout);

        ssize_t nread = getline(&line, &cap, stdin);
        if (nread == -1) {
            printf("\n");
            break;
        }

        int argc = parse_line(line, argv);
        int background = 0;
        if (argc > 0 && strcmp(argv[argc - 1], "&") == 0) {
            background = 1;
            argv[--argc] = NULL;
        }
        if (argc == 0) {
            continue;
        }

        if (strcmp(argv[0], "exit") == 0) {
            break;
        }
        if (strcmp(argv[0], "help") == 0) {
            printf("msh built-ins: cd [dir], help, exit\n");
            printf("Supports external commands, output redirection (>), and background jobs (&).\n");
            last_status = 0;
            continue;
        }
        if (strcmp(argv[0], "cd") == 0) {
            const char *target = argv[1];
            if (target == NULL) {
                target = getenv("HOME");
            }
            if (target == NULL || chdir(target) != 0) {
                perror("cd");
                last_status = 1;
            } else {
                last_status = 0;
            }
            continue;
        }

        pid_t pid = fork();
        if (pid < 0) {
            perror("fork");
            last_status = 1;
        } else if (pid == 0) {
            for (int i = 0; argv[i] != NULL; i++) {
                if (strcmp(argv[i], ">") == 0) {
                    if (argv[i + 1] == NULL) {
                        fprintf(stderr, "msh: syntax error near unexpected token `newline'\n");
                        _exit(1);
                    }
                    int fd = open(argv[i + 1], O_WRONLY | O_CREAT | O_TRUNC, 0644);
                    if (fd < 0) {
                        perror("open");
                        _exit(1);
                    }
                    if (dup2(fd, STDOUT_FILENO) < 0) {
                        perror("dup2");
                        close(fd);
                        _exit(1);
                    }
                    close(fd);
                    argv[i] = NULL;
                    break;
                }
            }
            execvp(argv[0], argv);
            perror(argv[0]);
            _exit(127);
        } else {
            if (background) {
                printf("[bg] started PID %d\n", pid);
                last_status = 0;
            } else {
                int status;
                if (waitpid(pid, &status, 0) < 0) {
                    perror("waitpid");
                    last_status = 1;
                } else if (WIFEXITED(status)) {
                    last_status = WEXITSTATUS(status);
                } else if (WIFSIGNALED(status)) {
                    last_status = 128 + WTERMSIG(status);
                }
            }
        }
    }

    free(line);
    return 0;
}