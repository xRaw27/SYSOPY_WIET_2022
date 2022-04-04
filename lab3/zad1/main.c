//
// Created by xraw on 30.03.2022.
//

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>


void error(char *message) {
    fprintf(stderr, "%s\n", message);
    exit(EXIT_FAILURE);
}


int main(int argc, char **argv) {
    if (argc != 2) error("One argument expected");

    for (int i = 0; i < atoi(argv[1]); ++i) {
        if (fork() == 0) {
            printf("PID: %d\n", getpid());
            return 0;
        }
    }

    return 0;
}