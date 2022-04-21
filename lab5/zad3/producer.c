//
// Created by xraw on 20.04.2022.
//

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <limits.h>
#include <fcntl.h>
#include <sys/file.h>
#include <time.h>


void error(char *message) {
    fprintf(stderr, "%s\n", message);
    exit(EXIT_FAILURE);
}

void produce(char *fifo_path, int row, char *file_path, int n) {
    FILE *fifo = fopen(fifo_path, "w");
    if (fifo == NULL) error("Error while opening fifo");

    FILE *in = fopen(file_path, "r");
    if (in == NULL) error("Error while opening input file");

    setvbuf(fifo, NULL, _IONBF, 0);

    unsigned long k;
    char row_string[6];
    char *buffer = calloc(n + 7, sizeof(char));
    char *n_buffer = &buffer[6];

    sprintf(row_string, "%d", row);
    for (unsigned long i = strlen(row_string); i < 6; ++i) {
        row_string[i] = '_';
    }
    strcpy(buffer, row_string);
    srand(time(NULL));

    while ((k = fread(n_buffer, 1, n, in)) > 0) {
        while (k < n) {
            n_buffer[k++] = '\0';
        }
        n_buffer[n] = '\0';
        printf(" *** BUFFER: %s\n", buffer);

        flock(fileno(fifo), LOCK_EX);
        fwrite(buffer, 1, n + 6, fifo);
        flock(fileno(fifo), LOCK_UN);

        usleep((long)((double)random()/RAND_MAX * 100000));
    }

    free(buffer);
    fclose(fifo);
    fclose(in);
}


int main(int argc, char **argv) {
    if (argc != 5) {
        error("Four arguments expected");
    }

    produce(argv[1], atoi(argv[2]), argv[3], atoi(argv[4]));
}