//
// Created by xraw on 20.04.2022.
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/file.h>

void error(char *message) {
    fprintf(stderr, "%s\n", message);
    exit(EXIT_FAILURE);
}

void save(FILE *output, char *text) {
    char row_string[7];
    strncpy(row_string, text, 6);
    row_string[6] = '\0';

    int row = atoi(row_string);
    text = &text[6];

    printf("%d - %s\n", row, text);

    fseek(output, 0, SEEK_END);
    unsigned long size = ftell(output);
    rewind(output);

    char *buffer = calloc(size + 1, sizeof(char));
    fread(buffer, 1, size, output);
    rewind(output);

    int line = 1;
    unsigned long i = 0;
    while (i < size) {
        if (buffer[i] == '\n') {
            if (line == row) {
                fwrite(text, 1, strlen(text), output);
            }
            ++line;
        }

        fwrite(&buffer[i], 1, 1, output);
        ++i;
    }
    if (line <= row) {
        while (line < row) {
            fwrite("\n", 1, 1, output);
            ++line;
        }
        fwrite(text, 1, strlen(text), output);
    }

    free(buffer);
}

void consume(char *fifo_path, char *file_path, int n) {
    FILE *fifo = fopen(fifo_path, "r");
    if (fifo == NULL) error("Error while opening fifo");

    FILE *out = fopen(file_path, "w+");
    if (out == NULL) error("Error while opening output file");

    setvbuf(fifo, NULL, _IONBF, 0);

    printf("\n<start>\n");

    char *buffer = calloc(n + 7, sizeof(char));
    while (1) {
        flock(fileno(out), LOCK_EX);
        if (fread(buffer, 1, n + 6, fifo) <= 0) {
            break;
        }
        save(out, buffer);
        flock(fileno(out), LOCK_UN);
    }

    printf("\n<end>\n");
    free(buffer);
    fclose(fifo);
    fclose(out);
}


int main(int argc, char **argv) {
    if (argc != 4) {
        error("Three arguments expected");
    }
    printf("%s %s %s\n", argv[1], argv[2], argv[3]);
    consume(argv[1], argv[2], atoi(argv[3]));
}
