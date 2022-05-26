//
// Created by xraw on 26.05.2022.
//

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <pthread.h>
#include <sys/time.h>


int width = 0;
int height = 0;
int **input_image;
int **output_image;

struct thread_args {
    pthread_t thread_id;
    int lower_limit;
    int upper_limit;
};


void invert_colors(int num_threads, char *mode);

void *numbers(void *arg);

void *blocks(void *arg);

void load_image(char *filename);

void save_image(char *filename);

void free_matrix(int **matrix, int h);

void error(char *message);


int main(int argc, char **argv) {
    if (argc != 5) {
        error("Four arguments expected");
    }

    char *input_path = argv[3];
    char *output_path = argv[4];

    load_image(input_path);

    invert_colors(atoi(argv[1]), argv[2]);

    save_image(output_path);

    free_matrix(input_image, height);
    free_matrix(output_image, height);

    return 0;
}


void invert_colors(int num_threads, char *mode) {
    struct timeval start, end;
    gettimeofday(&start, NULL);

    struct thread_args *t_info = calloc(num_threads, sizeof(struct thread_args));

    if (strcmp(mode, "numbers") == 0) {
        int range_size = 256 / num_threads;

        for (int t_num = 0; t_num < num_threads; ++t_num) {
            t_info[t_num].thread_id = t_num + 1;
            t_info[t_num].lower_limit = t_num * range_size;
            if (t_num == num_threads - 1) t_info[t_num].upper_limit = 256;
            else t_info[t_num].upper_limit = (t_num + 1) * range_size;

            pthread_create(&t_info[t_num].thread_id, NULL, &numbers, &t_info[t_num]);
        }
    }
    else if (strcmp(mode, "blocks") == 0) {
        int range_size = width / num_threads;

        for (int t_num = 0; t_num < num_threads; ++t_num) {
            t_info[t_num].thread_id = t_num + 1;
            t_info[t_num].lower_limit = t_num * range_size;
            if (t_num == num_threads - 1) t_info[t_num].upper_limit = width;
            else t_info[t_num].upper_limit = (t_num + 1) * range_size;

            pthread_create(&t_info[t_num].thread_id, NULL, &blocks, &t_info[t_num]);
        }
    }
    else {
        error("Wrong mode");
    }

    FILE *times = fopen("./Times.txt", "a");

    fprintf(times, "\n========== MODE: %s, THREADS: %d ==========\n\n", mode, num_threads);
    printf("\n========== MODE: %s, THREADS: %d ==========\n\n", mode, num_threads);

    printf("%-20s %-16s %-s\n", "thread_id", "range", "execution_time [s]");

    double *res;
    for (int t_num = 0; t_num < num_threads; ++t_num) {
        pthread_join(t_info[t_num].thread_id, (void **)&res);

        fprintf(times, "%-20lu [%4d, %4d)     %lf\n", t_info[t_num].thread_id, t_info[t_num].lower_limit, t_info[t_num].upper_limit, *res);
        printf("%-20lu [%4d, %4d)     %lf\n", t_info[t_num].thread_id, t_info[t_num].lower_limit, t_info[t_num].upper_limit, *res);

        free(res);
    }

    gettimeofday(&end, NULL);
    double total_execution_time = (double) (end.tv_usec - start.tv_usec) / 1000000 + (double) (end.tv_sec - start.tv_sec);

    fprintf(times, "\nTOTAL EXECUTION TIME: %lf [s]\n\n", total_execution_time);
    printf("\nTOTAL EXECUTION TIME: %lf [s]\n\n", total_execution_time );

    fclose(times);
    free(t_info);
}


void *numbers(void *arg) {
    struct timeval start, end;
    double *res = calloc(1, sizeof(long));

    gettimeofday(&start, NULL);

    struct thread_args *args = arg;

    for (int i = 0; i < height; ++i) {
        for (int j = 0; j < width; ++j) {
            if (input_image[i][j] >= args->lower_limit && input_image[i][j] < args->upper_limit) {
                output_image[i][j] = 255 - input_image[i][j];
            }
        }
    }

    gettimeofday(&end, NULL);
    *res = (double) (end.tv_usec - start.tv_usec) / 1000000 + (double) (end.tv_sec - start.tv_sec);

    return res;
}


void *blocks(void *arg) {
    struct timeval start, end;
    double *res = calloc(1, sizeof(long));

    gettimeofday(&start, NULL);

    struct thread_args *args = arg;

    for (int i = 0; i < height; ++i) {
        for (int j = args->lower_limit; j < args->upper_limit; ++j) {
            output_image[i][j] = 255 - input_image[i][j];
        }
    }

    gettimeofday(&end, NULL);
    *res = (double) (end.tv_usec - start.tv_usec) / 1000000 + (double) (end.tv_sec - start.tv_sec);

    return res;
}


void load_image(char *filename) {
    FILE *input_file = fopen(filename, "r");
    if (input_file == NULL) error("Error while opening file\n");

    int gray_level;
    char *image_info_buffer = calloc(128, sizeof(char));

    if (fgets(image_info_buffer, 100, input_file) == NULL) error("Wrong file format");
    if (fgets(image_info_buffer, 100, input_file) == NULL) error("Wrong file format");

    while (image_info_buffer[0] == '#') {
        if (fgets(image_info_buffer, 100, input_file) == NULL) error("Wrong file format");
    }
    sscanf(image_info_buffer, "%d %d", &width, &height);

    if (fgets(image_info_buffer, 100, input_file) == NULL) error("Wrong file format");
    sscanf(image_info_buffer, "%d", &gray_level);

    free(image_info_buffer);

    if (gray_level != 255) error("Gray level should be 255");

    printf("[loadfile] width=%d, height=%d, gray_level=%d\n", width, height, gray_level);

    input_image = calloc(height, sizeof(int *));
    output_image = calloc(height, sizeof(int *));
    for (int i = 0; i < height; ++i) {
        input_image[i] = calloc(width, sizeof(int));
        output_image[i] = calloc(width, sizeof(int));
    }

    char num_buffer[4] = "";
    char c;
    int w = 0, h = 0;
    while (fread(&c, 1, 1, input_file) > 0) {
        if (c == ' ' || c == '\n') {
            if (num_buffer[0] != '\0') {
                input_image[h][w++] = atoi(num_buffer);
                num_buffer[0] = '\0';

                if (w == width) {
                    w = 0;
                    ++h;
                }
                if (h == height) break;
            }
        } else {
            strncat(num_buffer, &c, 1);
        }
    }

    fclose(input_file);
}


void save_image(char *filename) {
    FILE *output_file = fopen(filename, "w");
    if (output_file == NULL) error("Error while opening file\n");

    fprintf(output_file, "P2\n");
    fprintf(output_file, "# negative output file\n");
    fprintf(output_file, "%d %d\n", width, height);
    fprintf(output_file, "%d\n", 255);

    for (int i = 0; i < height; ++i) {
        for (int j = 0; j < width; ++j) {
            fprintf(output_file, "%d ", output_image[i][j]);
        }
        fprintf(output_file, "\n");
    }

    fclose(output_file);
}


void free_matrix(int **matrix, int h) {
    if (h == 0) return;

    for (int i = 0; i < h; ++i) {
        free(matrix[i]);
    }
    free(matrix);
}


void error(char *message) {
    if (errno != 0) {
        perror(message);
    }
    else {
        fprintf(stderr, "[error] %s\n", message);
    }
    exit(EXIT_FAILURE);
}