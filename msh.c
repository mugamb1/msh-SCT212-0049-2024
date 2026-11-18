#define _POSIX_C_SOURCE 200809L
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>


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

    while (1) {
        printf("msh> ");
        fflush(stdout);

        ssize_t nread = getline(&line, &cap, stdin);
        if (nread == -1) {
            printf("\n");
            break;
        }

        int argc = parse_line(line, argv);
        if (argc == 0) {
            continue;
        }
        if (strcmp(argv[0], "exit") == 0) {
            break;
        }
        if (strcmp(argv[0], "help") == 0) {
            printf("msh built-ins: cd [dir], help, exit\n");
            printf("Supports external commands, output redirection (>), and background jobs (&).\n");
            continue;
        }
        if (strcmp(argv[0], "cd") == 0) {
            const char *target = argv[1];
            if (target == NULL) {
                target = getenv("HOME");
            }
            if (target == NULL || chdir(target) != 0) {
                perror("cd");
            }
            continue;
        }
        pid_t pid = fork();
        if (pid < 0) {
            perror("fork");
        } else if (pid == 0) {
            execvp(argv[0], argv);
            perror(argv[0]);
            _exit(127);
        } else {
            int status;
            if (waitpid(pid, &status, 0) < 0) {
                perror("waitpid");
            }
        }    
    }

    free(line);
    return 0;
}