//
// Created by xraw on 19.03.2022.
//

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>
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


void no_buffer_remove_empty(int in, int out, int count, int empty_line) {
    char c = ' ';

    while(1) {
        if (read(in, &c, 1) == 0) {
            c = '\n';
        } else {
            ++count;
        }

        if (c == '\n') {
            if (!empty_line) {
                lseek(in, -count, SEEK_CUR);
                for (int i = 0; i < count; ++i) {
                    if (read(in, &c, 1) == 0) {
                        error("Error while reading file");
                    }
                    if (write(out, &c, 1) == -1) {
                        error("Error while writing to file");
                    }
                }
            }
            return;
        }
        else if (c != ' ' && empty_line) {
            empty_line = 0;
        }
    }
}


void buffer_remove_empty(int in, int out, char *in_buffer, char *out_buffer, int count) {
    int p = 0;
    int empty_line = 1;
    int out_buff_index = 0;

    for (int i = 0; i < count; ++i) {
        if (in_buffer[i] == '\n') {
            if (!empty_line) {
                for (;p <= i; ++p) {
                    out_buffer[out_buff_index++] = in_buffer[p];
                }
                empty_line = 1;
            }
            p = i + 1;
        }
        else if (in_buffer[i] != ' ' && empty_line) {
            empty_line = 0;
        }
    }

    if (p == 0) {
        // when single line is longer than buffer program reads from file char by char
        no_buffer_remove_empty(in, out, count, empty_line);
    }
    else {
        if (write(out, out_buffer, out_buff_index) == -1) {
            error("Error while writing to file");
        }
        lseek(in, p - count, SEEK_CUR);
    }
}


void remove_empty_lines(char *in_name, char *out_name, int buffer_size) {
    char in_buffer[buffer_size];
    char out_buffer[buffer_size];
    int in, out, count;

    in = open(in_name, O_RDONLY);
    if (in == -1) {
        error("Error while opening input file");
    }
    out = open(out_name, O_WRONLY|O_TRUNC|O_CREAT, S_IRUSR|S_IWUSR);
    if (out == -1) {
        close(in);
        error("Error while opening output file");
    }

    while((count = read(in, in_buffer, buffer_size)) > 0) {
        if (count < buffer_size) {
            in_buffer[count++] = '\n';
        }
        buffer_remove_empty(in, out, in_buffer, out_buffer, count);
    }

    close(in);
    close(out);
}


int main(int argc, char **argv) {
    int buffer_size = 102400;
    char in_name[256] = {};
    char out_name[256] = {};

    static struct option long_options[] = {
            {"input",        required_argument,  0,  'i' },
            {"output",       required_argument,  0,  'o' },
            {"buffer-size",  required_argument,  0,  'b' },
    };
    int opt, long_index = 0;

    while ((opt = getopt_long(argc, argv,"i:o:b:",long_options, &long_index )) != -1) {
        switch (opt) {
            case 'i':
                strcpy(in_name, optarg);
                break;
            case 'o':
                strcpy(out_name, optarg);
                break;
            case 'b':
                buffer_size = atoi(optarg);
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
    printf("[main] Program options:\nin_name=%s\nout_name=%s\nbuffer_size=%d\n", in_name, out_name, buffer_size);

    remove_empty_lines(in_name, out_name, buffer_size);

    return 0;
}
