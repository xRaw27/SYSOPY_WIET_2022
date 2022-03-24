//
// Created by xraw on 24.03.2022.
//

#define _XOPEN_SOURCE 500
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <string.h>
#include <linux/limits.h>
#include <ftw.h>


struct overall_statistics {
    size_t all;
    size_t files;
    size_t dirs;
    size_t char_devs;
    size_t block_devs;
    size_t fifos;
    size_t slinks;
    size_t sockets;
};

struct overall_statistics overall_stats = {0, 0, 0, 0, 0, 0};

void error(char *message) {
    fprintf(stderr, "%s\n", message);
    exit(EXIT_FAILURE);
}

void get_time(time_t time, char *buffer) {
    strftime(buffer, 20, "%X %d/%m/%Y", localtime(&time));
}

char *file_type(const mode_t *m, struct overall_statistics *stats) {
    stats->all+= 1;
    if (S_ISREG(*m)) {
        stats->files += 1;
        return "FILE";
    }
    if (S_ISDIR(*m)) {
        stats->dirs += 1;
        return "DIR";
    }
    if (S_ISCHR(*m)) {
        stats->char_devs += 1;
        return "CHAR DEV";
    }
    if (S_ISBLK(*m)) {
        return "BLOCK DEV";
    }
    if (S_ISFIFO(*m)) {
        stats->fifos += 1;
        return "FIFO";
    }
    if (S_ISLNK(*m)) {
        stats->slinks += 1;
        return "SLINK";
    }
    if (S_ISSOCK(*m)) {
        stats->sockets += 1;
        return "SOCKET";
    }
    return "OTHER";
}

void print_header() {
    printf("\n %-8s %-10s %-10s %-24s %-24s %-s\n\n", "Links", "File type", "Size [B]", "Last access time", "Last modification time", "Absolute path");
}

int print_file_status(char *path, struct stat *file_status, int flag, struct FTW *ftw_buf) {
    char atime_buffer[20];
    char mtime_buffer[20];
    get_time(file_status->st_atime, atime_buffer);
    get_time(file_status->st_mtime, mtime_buffer);
    printf(" %-8lu %-10s %-10lu %-24s %-24s %-s\n", file_status->st_nlink, file_type(&file_status->st_mode, &overall_stats), file_status->st_size, atime_buffer, mtime_buffer, path);

    return 0;
}

void print_overall_stats(struct overall_statistics *stats) {
    printf("\n\n %-7s%-8s%-7s%-12s%-13s%-8s%-9s%-10s\n", "ALL", "FILES", "DIRS", "CHAR DEVS", "BLOCK DEVS", "FIFOS", "SLINKS", "SOCKETS");
    printf(" %-7lu%-8lu%-7lu%-12lu%-13lu%-8lu%-9lu%-10lu\n", stats->all, stats->files, stats->dirs, stats->char_devs, stats->block_devs, stats->fifos, stats->slinks, stats->sockets);
}

int main(int argc, char **argv) {
    if (argc != 2) {
        error("One argument expected");
    }

    char path[PATH_MAX + 1];
    realpath(argv[1], path);

    print_header();
    nftw(path, (__nftw_func_t) print_file_status, 20, FTW_PHYS);
    print_overall_stats(&overall_stats);

    return 0;
}