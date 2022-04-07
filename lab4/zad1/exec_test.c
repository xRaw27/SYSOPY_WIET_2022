//
// Created by xraw on 05.04.2022.
//

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <signal.h>
#include <string.h>

void signal_handler(int signum) {
    printf("[PID %d process] Received signal: %d\n", getpid(), signum);
}

int main(int argc, char **argv) {
    if (argc != 2) {
        exit(EXIT_FAILURE);
    }

    if (strcmp(argv[1], "ignore") == 0) {
        printf("[PID %d process] Exec program sends a signal\n", getpid());
        fflush(stdout);
        raise(SIGUSR1);
    }
    else if (strcmp(argv[1], "handler") == 0) {
        printf("[PID %d process] Exec program sends a signal\n", getpid());
        fflush(stdout);
        raise(SIGUSR1);
    }
    else if (strcmp(argv[1], "mask") == 0) {
        sigset_t pending;
        raise(SIGUSR1);
        printf("[PID %d process] Exec program sent signal\n", getpid());
        sigpending(&pending);
        printf("[PID %d process] Is SIGUSR1 pending (exec program): %s\n", getpid(), sigismember(&pending, SIGUSR1) ? "YES" : "NO");
    }
    else if (strcmp(argv[1], "pending") == 0) {
        sigset_t pending;
        sigpending(&pending);
        printf("[PID %d process] Is SIGUSR1 from main program pending (exec program): %s\n", getpid(), sigismember(&pending, SIGUSR1) ? "YES" : "NO");
    }
    else {
        exit(EXIT_FAILURE);
    }

    return 0;
}