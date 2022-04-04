//
// Created by xraw on 30.03.2022.
//

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <dirent.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <string.h>
#include <linux/limits.h>


void error(char *message) {
    fprintf(stderr, "%s\n", message);
    exit(EXIT_FAILURE);
}

int is_text_file(char *filename) {
    char *buffer = calloc(44 + strlen(filename), sizeof(char));
    sprintf(buffer, "file --mime-type %s | sed 's|.*: ||; s|/.*||'", filename);

    FILE *fptr;
    fptr = popen(buffer, "r");
    if (fptr == NULL) error("Failed to run `file` command");

    fread(buffer, 4, 1, fptr);
    pclose(fptr);
    buffer[4] = '\0';

    if (strcmp(buffer, "text") == 0) {
        free(buffer);
        return 1;
    }
    free(buffer);
    return 0;
}


int check_file(char *filepath, char *pattern, int pattern_len) {
    FILE *fptr = fopen(filepath, "r");
    if (!fptr) error("Error while opening input file");

    char *buffer = calloc(pattern_len + 1, sizeof(char));

    while(fread(buffer, 1, pattern_len, fptr) == pattern_len) {
        if (strcmp(buffer, pattern) == 0) {
            free(buffer);
            fclose(fptr);
            return 1;
        }
        fseek(fptr, -pattern_len + 1, SEEK_CUR);
    }
    free(buffer);
    fclose(fptr);
    return 0;
}


void traverse(char *path, int init_path_len, char *pattern, int pattern_len, int max_depth) {
    char temp_path[PATH_MAX + 1];
    struct stat file_status;
    struct dirent* dir_entry;

    strcat(path, "/");

    DIR* current_dir = opendir(path);
    if (current_dir == NULL) {
        printf("%s %s\n", "Could not open directory", path);
        return;
    }

    while ((dir_entry = readdir(current_dir)) != NULL) {
        if (strcmp(dir_entry->d_name, ".") != 0 && strcmp(dir_entry->d_name, "..") != 0) {
            strcpy(temp_path, path);
            strcat(temp_path, dir_entry->d_name);

            lstat(temp_path, &file_status);

            if (S_ISDIR(file_status.st_mode) && max_depth > 0) {
                if (fork() == 0) {
                    closedir(current_dir);
                    traverse(temp_path, init_path_len, pattern, pattern_len, max_depth - 1);
                    exit(EXIT_SUCCESS);
                }
            }

            if (S_ISREG(file_status.st_mode)) {
                if (is_text_file(temp_path) && check_file(temp_path, pattern, pattern_len)) {
                    printf("File contains pattern -> PID: %d | PATH: .%s \n", getpid(), &temp_path[init_path_len]);
                }
            }
        }
    }

    closedir(current_dir);
}


int main(int argc, char **argv) {
    if (argc != 4) {
        error("Three arguments expected");
    }

    char path[PATH_MAX + 1] = ".";
    int init_path_len;

    realpath(argv[1], path);
    init_path_len = (int)strlen(path);
    traverse(path, init_path_len, argv[2], (int)strlen(argv[2]), atoi(argv[3]));

    return 0;
}
