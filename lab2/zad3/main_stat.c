//
// Created by xraw on 23.03.2022.
//
#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <time.h>
#include <string.h>
#include <linux/limits.h>


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

void print_file_status(struct stat *file_status, struct overall_statistics *stats, char *path, char *atime_buffer, char *mtime_buffer) {
    get_time(file_status->st_atime, atime_buffer);
    get_time(file_status->st_mtime, mtime_buffer);
    printf(" %-8lu %-10s %-10lu %-24s %-24s %-s\n", file_status->st_nlink, file_type(&file_status->st_mode, stats), file_status->st_size, atime_buffer, mtime_buffer, path);
}

void print_overall_stats(struct overall_statistics *stats) {
    printf("\n\n %-7s%-8s%-7s%-12s%-13s%-8s%-9s%-10s\n", "ALL", "FILES", "DIRS", "CHAR DEVS", "BLOCK DEVS", "FIFOS", "SLINKS", "SOCKETS");
    printf(" %-7lu%-8lu%-7lu%-12lu%-13lu%-8lu%-9lu%-10lu\n", stats->all, stats->files, stats->dirs, stats->char_devs, stats->block_devs, stats->fifos, stats->slinks, stats->sockets);
}

void traverse(char *path, struct stat *file_status, struct overall_statistics *overall_stats, char *atime_buffer, char *mtime_buffer) {
    char temp_path[PATH_MAX + 1];
    strcat(path, "/");

//    printf("[path] %s\n\n", path);
    struct dirent* dir_entry;
    DIR* current_dir = opendir(path);
    if (current_dir == NULL) {
        printf("%s %s\n", "Could not open directory", path);
        return;
    }

    while ((dir_entry = readdir(current_dir)) != NULL) {
        if (strcmp(dir_entry->d_name, ".") != 0 && strcmp(dir_entry->d_name, "..") != 0) {
            strcpy(temp_path, path);
            strcat(temp_path, dir_entry->d_name);

            lstat(temp_path, file_status);
            print_file_status(file_status, overall_stats, temp_path, atime_buffer, mtime_buffer);

            if (S_ISDIR(file_status->st_mode)) {
                traverse(temp_path, file_status, overall_stats, atime_buffer, mtime_buffer);
            }
        }
    }

    closedir(current_dir);
}

int main(int argc, char **argv) {
    if (argc != 2) {
        error("One argument expected");
    }

    struct stat file_status;
    struct overall_statistics overall_stats = {0, 0, 0, 0, 0, 0};
    char atime_buffer[20];
    char mtime_buffer[20];
    char path[PATH_MAX + 1];

    realpath(argv[1], path);
    print_header();

    if (lstat(path, &file_status) != -1) {
        print_file_status(&file_status, &overall_stats, path, atime_buffer, mtime_buffer);
    }

    traverse(path, &file_status, &overall_stats, atime_buffer, mtime_buffer);
    print_overall_stats(&overall_stats);

    return 0;
}
