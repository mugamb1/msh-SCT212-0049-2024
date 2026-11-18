#define _POSIX_C_SOURCE 200809L
#include <stdio.h>
#include <stdlib.h>

int main(void) {
    char *line = NULL;
    size_t cap = 0;

    while (1) {
        printf("msh> ");
        fflush(stdout);

        ssize_t nread = getline(&line, &cap, stdin);
        if (nread == -1) {
            printf("\n");
            break;
        }
    }

    free(line);
    return 0;
}
