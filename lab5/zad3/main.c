//
// Created by xraw on 20.04.2022.
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <time.h>

void error(char *message) {
    fprintf(stderr, "%s\n", message);
    exit(EXIT_FAILURE);
}


void check(char **rows, char **files, char *output, int k) {
    int result = 0;

    FILE *out = fopen(output, "r");
    if (out == NULL) error("Error while opening output file");

    fseek(out, 0, SEEK_END);
    unsigned long size = ftell(out);
    rewind(out);

    char *buffer = calloc(size + 1, sizeof(char));
    fread(buffer, 1, size, out);

    for (int i = 0; i < k; ++i) {
        FILE *in = fopen(files[i], "r");
        if (in == NULL) error("Error while opening input file");

        fseek(in, 0, SEEK_END);
        unsigned long size2 = ftell(in);
        rewind(in);

        char *buffer2 = calloc(size2 + 1, sizeof(char));
        char *buffer3 = calloc(size2 + 1, sizeof(char));
        fread(buffer2, 1, size2, in);

        int row = atoi(rows[i]) - 1;
        int p = 0;
        while (row > 0) {
            if (buffer[p] == '\n') {
                --row;
            }
            ++p;
        }
        strncpy(buffer3, &buffer[p], size2);
        if (strcmp(buffer3, buffer2) != 0) {
            result = 1;
        }

        free(buffer2);
        free(buffer3);
        fclose(in);
    }

    free(buffer);
    fclose(out);

    FILE *res = fopen("./wnioski.txt", "a+");
    if (result == 0) fprintf(res, " *** TEST %s POPRAWNY\n", output);
    else fprintf(res, " *** TEST %s NIEPOPRAWNY\n", output);
}


void test1(char *n, char *output, int short_mode) {
    mkfifo("./fifo", 0664);

    if (fork() == 0) {
        char *args[] = {"./consumer", "./fifo", output, n, NULL};
        execvp(args[0], args);
    }

    char *rows[] = {"5", "3", "10", "50", "2"};
    char *files[] = {"./files/a.txt", "./files/b.txt", "./files/c.txt", "./files/d.txt", "./files/e.txt"};
    char *files_short[] = {"./files/a_short.txt", "./files/b_short.txt", "./files/c_short.txt", "./files/d_short.txt", "./files/e_short.txt"};

    for (int i = 0; i < 5; ++i) {
        if (fork() == 0) {
            if (short_mode == 1) {
                char *args[] = {"./producer", "./fifo", rows[i], files_short[i], n, NULL};
                execvp(args[0], args);
            } else {
                char *args[] = {"./producer", "./fifo", rows[i], files[i], n, NULL};
                execvp(args[0], args);
            }
        }
    }

    while(wait(NULL) > 0);

    if (short_mode == 1) {
        check(rows, files_short, output, 5);
    } else {
        check(rows, files, output, 5);
    }
}

void test2(char *n, char *output, int short_mode) {
    mkfifo("./fifo", 0664);

    for (int i = 0; i < 2; ++i) {
        if (fork() == 0) {
            char *args[] = {"./consumer", "./fifo", output, n, NULL};
            execvp(args[0], args);
        }
    }

    char *rows[] = {"5"};
    char *files[] = {"./files/a.txt"};
    char *files_short[] = {"./files/a_short.txt"};

    if (fork() == 0) {
        if (short_mode == 1) {
            char *args[] = {"./producer", "./fifo", rows[0], files_short[0], n, NULL};
            execvp(args[0], args);
        } else {
            char *args[] = {"./producer", "./fifo", rows[0], files[0], n, NULL};
            execvp(args[0], args);
        }
    }

    while(wait(NULL) > 0);

    if (short_mode == 1) {
        check(rows, files_short, output, 1);
    } else {
        check(rows, files, output, 1);
    }
}

void test3(char *n, char *output, int short_mode) {
    mkfifo("./fifo", 0664);

    for (int i = 0; i < 2; ++i) {
        if (fork() == 0) {
            char *args[] = {"./consumer", "./fifo", output, n, NULL};
            execvp(args[0], args);
        }
    }

    char *rows[] = {"5", "3", "10", "50", "2"};
    char *files[] = {"./files/a.txt", "./files/b.txt", "./files/c.txt", "./files/d.txt", "./files/e.txt"};
    char *files_short[] = {"./files/a_short.txt", "./files/b_short.txt", "./files/c_short.txt", "./files/d_short.txt", "./files/e_short.txt"};

    for (int i = 0; i < 5; ++i) {
        if (fork() == 0) {
            if (short_mode == 1) {
                char *args[] = {"./producer", "./fifo", rows[i], files_short[i], n, NULL};
                execvp(args[0], args);
            } else {
                char *args[] = {"./producer", "./fifo", rows[i], files[i], n, NULL};
                execvp(args[0], args);
            }
        }
    }

    while(wait(NULL) > 0);

    if (short_mode == 1) {
        check(rows, files_short, output, 5);
    } else {
        check(rows, files, output, 5);
    }
}



int main() {
    test1("5", "./files/test_1_n_5.txt", 1);
    test1("1000", "./files/test_1_n_1000.txt", 0);
    test1("4200", "./files/test_1_n_4200.txt", 0);

    test2("5", "./files/test_2_n_5.txt", 1);
    test2("1000", "./files/test_2_n_1000.txt", 0);
    test2("4200", "./files/test_2_n_4200.txt", 0);

    test3("5", "./files/test_3_n_5.txt", 1);
    test3("1000", "./files/test_3_n_1000.txt", 0);
    test3("4200", "./files/test_3_n_4200.txt", 0);
}
