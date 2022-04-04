//
// Created by xraw on 30.03.2022.
//

#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>
#include <math.h>
#include <sys/wait.h>


void error(char *message) {
    fprintf(stderr, "%s\n", message);
    exit(EXIT_FAILURE);
}


double f(double const x) {
    return (double) 4 / (x * x + 1);
}


double calc_rectangle(double const a, double const b) {
    return f((a + b) / 2) * (b - a);
}


void calc_in_new_process(const double * const width, long unsigned const block_start, long unsigned const block_end, int n) {
    if (fork() == 0) {
        double sum = 0;

        if ((double) block_start * (*width) < (double) 1) {
            for (long unsigned i = block_start; i < block_end; ++i) {

                if ((double)(i + 1) * (*width) > (double) 1) {
                    sum += calc_rectangle((double)i * (*width), (double) 1);
                    break;
                }
                sum += calc_rectangle((double)i * (*width), (double)(i + 1) * (*width));

            }
        }

        char filename[20];
        sprintf(filename, "w%d.txt", n);

        FILE *fptr = fopen(filename, "w");
        if (!fptr) error("Error while opening file");
        fprintf(fptr, "%.50lf", sum);
        fclose(fptr);

        exit(EXIT_SUCCESS);
    }
}


double sum_results(int n) {
    char filename[20];
    sprintf(filename, "w%d.txt", n);

    FILE *fptr = fopen(filename, "r");
    if (!fptr) error("Error while opening file");

    fseek(fptr, 0, SEEK_END);
    size_t len = ftell(fptr);
    fseek(fptr, 0, SEEK_SET);

    char string_sum[len + 1];
    fread(string_sum, len, 1, fptr);
    string_sum[len] = '\0';

    fclose(fptr);

    return atof(string_sum);
}


int main(int argc, char **argv) {
    long unsigned rectangles, block_size, block_start, block_end;
    double width, sum;
    int n;

    if (argc != 3) error("Two arguments expected");

    width = atof(argv[1]);
    n = atoi(argv[2]);

    if (width <= (double) 0 || width > (double) 1 ) error("Rectangle width must be in range (0; 1]");
    if (n <= 0) error("Number of child processes must be greater than zero");

    rectangles = floor(1 / width);
    block_size = rectangles / n;
    if (rectangles % n > 0) ++block_size;

    for (int i = 0; i < n; ++i) {
        block_start = i * block_size;
        block_end = (i + 1) * block_size;
        if (block_end == rectangles) ++block_end;

        calc_in_new_process(&width, block_start, block_end, i + 1);
        wait(NULL);
    }

    while (wait(NULL) > 0);

    sum = 0;
    for (int i = 0; i < n; ++i) {
        sum += sum_results(i + 1);
    }

    printf("Result = %.32lf\n", sum);

    return 0;
}

