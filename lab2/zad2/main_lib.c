//
// Created by xraw on 20.03.2022.
//
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define BUFFER_SIZE 10240

struct result {
    int chars_count;
    int lines_count;
};

void error(char *message) {
    fprintf(stderr, "%s\n", message);
    exit(EXIT_FAILURE);
}

void print_res(struct result *res) {
    printf("result = { \n    chars_count = %d\n    lines_count = %d\n}\n", res->chars_count, res->lines_count);
}

struct result count_char(char *file_name, char c) {
    char buffer[BUFFER_SIZE];
    struct result res = {0, 0};

    int count, p, chars_in_line;
    FILE *in = fopen(file_name, "r");
    if (!in) {
        error("Error while opening input file");
    }

    while((count = fread(buffer, 1, BUFFER_SIZE, in)) > 0) {
        if (count < BUFFER_SIZE) {
            buffer[count++] = '\n';
        }
        p = 0;
        chars_in_line = 0;
        for (int i = 0; i < count; ++i) {
            if (buffer[i] == c) {
                chars_in_line += 1;
            }
            else if (buffer[i] == '\n') {
                p = i + 1;
                if (chars_in_line > 0) {
                    res.chars_count += chars_in_line;
                    res.lines_count += 1;
                    chars_in_line = 0;
                }
            }
        }
        fseek(in, p - count, SEEK_CUR);
    }

    fclose(in);
    return res;
}


int main(int argc, char **argv) {

    if (argc != 3) {
        error("Two arguments expected");
    }
    if (strlen(argv[2]) != 1) {
        error("2nd argument should be char");
    }

    struct result res = count_char(argv[1], argv[2][0]);
    print_res(&res);

    return 0;
}