//
// Created by xraw on 19.03.2022.
//

#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <string.h>


void print_buffer(char *arr, int n) {
    printf("[print_buffer]: ");
    for (int i = 0; i < n; ++i) {
        if (arr[i] > 32) {
            printf("%2c ", arr[i]);
        } else {
            printf("%2d ", arr[i]);
        }
    }
    printf("\n");
}


void error(char *message) {
    fprintf(stderr, "%s\n", message);
    exit(EXIT_FAILURE);
}


void remove_empty_lines(char *in_name, char *out_name) {
    FILE *in = fopen(in_name, "r");
    if (!in) {
        error("Error while opening input file");
    }
    FILE *out = fopen(out_name, "w");
    if (!out) {
        fclose(in);
        error("Error while opening output file");
    }

    char c;
    int empty_line = 1, count = 0, eof = 0;

    while(!eof) {
        if (fread(&c, 1, 1, in) == 0) {
            c = '\n';
            eof = 1;
        } else {
            ++count;
        }

        if (c == '\n') {
            if (!empty_line) {
                fseek(in, -count, SEEK_CUR);
                for (;count > 0; --count) {
                    if (fread(&c, 1, 1, in) != 1) {
                        error("Error while reading file");
                    }
                    if (fwrite(&c, 1, 1, out) != 1) {
                        error("Error while writing to file");
                    }
                }
                empty_line = 1;
            }
            count = 0;
        }
        else if (c != ' ' && empty_line) {
            empty_line = 0;
        }
    }
    fclose(in);
    fclose(out);
}


int main(int argc, char **argv) {
    char in_name[256] = {};
    char out_name[256] = {};

    static struct option long_options[] = {
            {"input",        required_argument,  0,  'i' },
            {"output",       required_argument,  0,  'o' },
    };
    int opt, long_index = 0;

    while ((opt = getopt_long(argc, argv,"i:o:",long_options, &long_index )) != -1) {
        switch (opt) {
            case 'i':
                strcpy(in_name, optarg);
                break;
            case 'o':
                strcpy(out_name, optarg);
                break;
            default:
                exit(EXIT_FAILURE);
        }
    }

    if (in_name[0] == '\0') {
        printf("Input file name: ");
        if(scanf("%255s", in_name) <= 0) {
            error("Wrong input");
        }
    }
    if (out_name[0] == '\0') {
        printf("Output file name: ");
        if(scanf("%255s", out_name) <= 0) {
            error("Wrong input");
        }
    }
    printf("[main] Program options:\nin_name=%s\nout_name=%s\n", in_name, out_name);

    remove_empty_lines(in_name, out_name);

    return 0;
}
