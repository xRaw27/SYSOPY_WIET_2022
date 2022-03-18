//
// Created by xraw on 15.03.2022.
//
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <getopt.h>
#include <sys/times.h>
#include <unistd.h>
#include "lib.h"

struct tms start_tms;
struct tms end_tms;
clock_t start_time;
clock_t end_time;
clock_t tics_per_second;

void timer_start() {
    start_time = times(&start_tms);
}

void timer_end() {
    end_time = times(&end_tms);
}

void init_results(char *results_path) {
    FILE *fptr;
    fptr = fopen (results_path, "a");
    fprintf(fptr, "\n %13s | %13s | %15s | %s \n", "Real time [s]", "User time [s]", "System time [s]", "Timer description ");
    fprintf(fptr, "---------------|---------------|-----------------|--------------------------------------------------\n");
    fclose(fptr);
}

void update_results(char *results_path, char *description) {
    double real_time = (double)(end_time - start_time)/tics_per_second;
    double user_time = (double)(end_tms.tms_utime - start_tms.tms_utime + end_tms.tms_cutime - start_tms.tms_cutime)/tics_per_second;
    double system_time = (double)(end_tms.tms_stime - start_tms.tms_stime + end_tms.tms_cstime - start_tms.tms_cstime)/tics_per_second;

    FILE *fptr;
    fptr = fopen (results_path, "a");
    fprintf(fptr, " %13f | %13f | %15f | %s \n", real_time, user_time, system_time, description);
    fclose(fptr);
}

int count_files(char *p) {
    int counter = 1;

    while (*p != '\0') {
        if (*p == ',') {
            ++counter;
        }
        ++p;
    }

    return counter;
}

void wc_files() {
    int files_count = count_files(optarg);
    char **file_names = calloc(files_count, sizeof(char *));

    int  i = 0;
    char *p = strtok(optarg, ",");
    while(p != NULL) {
        file_names[i++] = p;
        p = strtok(NULL, ",");
    }

    wc_to_temp(files_count, file_names);

    free(file_names);
}

int main(int argc, char **argv) {
    tics_per_second = sysconf(_SC_CLK_TCK);
    int save_results = 0;
    char *results_path;

    static struct option long_options[] = {
            {"create_table",        required_argument,  0,  'c' },
            {"wc_files",            required_argument,  0,  'w' },
            {"save_block",          no_argument,        0,  'b' },
            {"save_block_n_times",  required_argument,  0,  'n' },
            {"remove_block",        required_argument,  0,  'r' },
            {"remove_m_blocks",     required_argument,  0,  'm' },
            {"print_table",         no_argument,        0,  'p' },
            {"timer_start",         no_argument,        0,  't' },
            {"timer_end",           required_argument,  0,  'e' },
            {"save_results",        required_argument,  0,  's' },
    };
    int opt;
    int long_index = 0;
    while ((opt = getopt_long(argc, argv,"c:w:bn:r:m:pte:s:",long_options, &long_index )) != -1) {
        switch (opt) {
            case 'c':
                printf("[main] --create_table %s\n", optarg);
                create_table(atoi(optarg));
                break;
            case 'w':
                printf("[main] --wc_files %s\n", optarg);
                wc_files();
                break;
            case 'b':
                printf("[main] --save_block\n");
                save_block(0);
                break;
            case 'n':
                printf("[main] --save_n_blocks %s\n", optarg);
                for (size_t i = 0; i < atoi(optarg); i++) {
                    save_block(1);
                }
                break;
            case 'r':
                printf("[main] --remove_block %s\n", optarg);
                break;
            case 'm':
                printf("[main] --remove_m_blocks");
                char *p = strtok(optarg, " ");
                size_t m = atoi(p);
                p = strtok(NULL, " ");
                size_t index = atoi(p);
                printf(" %lu %lu\n", m, index);

                for (size_t i = index; i < index + m; i++) {
                    delete_block(i);
                }
                break;
            case 'p':
                print_table();
                break;
            case 't':
                printf("[main] --timer_start\n");
                timer_start();
                break;
            case 'e':
                printf("[main] --timer_end %s\n", optarg);
                timer_end();
                if (save_results) {
                    update_results(results_path, optarg);
                }
                break;
            case 's':
                if (!save_results) {
                    save_results = 1;
                    results_path = optarg;
                    init_results(results_path);
                }
                break;
            default:
                exit(EXIT_FAILURE);
        }
    }

    if (save_results) {
        printf("[main] Saving results to %s\n", results_path);
    }

    free_table();

    return 0;
}
