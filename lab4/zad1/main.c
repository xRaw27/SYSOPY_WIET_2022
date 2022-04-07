//
// Created by xraw on 04.04.2022.
//

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <signal.h>
#include <string.h>
#include <linux/limits.h>


void error(char *message) {
    fprintf(stderr, "%s\n", message);
    exit(EXIT_FAILURE);
}


void signal_handler(int signum) {
    printf("[PID %d process] [HANDLER] Received signal: %d\n", getpid(), signum);
    fflush(stdout);
}

void test_ignore(int mode) {
    signal(SIGUSR1, SIG_IGN);
    printf("[PID %d process] SIGUSR1 signals are now ignored\n", getpid());
    printf("[PID %d process] Parent/main sends a signal\n", getpid());
    fflush(stdout);
    raise(SIGUSR1);

    if (mode == 0 && fork() == 0) {
        printf("[PID %d process] Child sends a signal\n", getpid());
        raise(SIGUSR1);
    }
}

void test_handler(int mode) {
    signal(SIGUSR1, signal_handler);
    printf("[PID %d process] Handler for SIGUSR1 signals has been set\n", getpid());
    printf("[PID %d process] Parent/main sends a signal\n", getpid());
    fflush(stdout);
    raise(SIGUSR1);

    if (mode == 0 && fork() == 0) {
        printf("[PID %d process] Child sends a signal\n", getpid());
        raise(SIGUSR1);
    }
}

void test_mask(int mode) {
    sigset_t mask, pending;
    sigaddset(&mask, SIGUSR1);

    sigprocmask(SIG_SETMASK, &mask, NULL);
    printf("[PID %d process] Mask has been set\n", getpid());
    raise(SIGUSR1);
    printf("[PID %d process] Parent/main sent signal\n", getpid());
    sigpending(&pending);
    printf("[PID %d process] Is SIGUSR1 pending (parent/main): %s\n", getpid(), sigismember(&pending, SIGUSR1) ? "YES" : "NO");
    fflush(stdout);

    if (mode == 0 && fork() == 0) {
        raise(SIGUSR1);
        printf("[PID %d process] Child sent signal\n", getpid());
        sigpending(&pending);
        printf("[PID %d process] Is SIGUSR1 pending (child): %s\n", getpid(), sigismember(&pending, SIGUSR1) ? "YES" : "NO");
    }
}

void test_pending(int mode) {
    sigset_t mask, pending;
    sigaddset(&mask, SIGUSR1);

    sigprocmask(SIG_SETMASK, &mask, NULL);
    printf("[PID %d process] Mask has been set\n", getpid());
    raise(SIGUSR1);
    printf("[PID %d process] Parent/main sent signal\n", getpid());
    sigpending(&pending);
    printf("[PID %d process] Is SIGUSR1 pending (parent/main): %s\n", getpid(), sigismember(&pending, SIGUSR1) ? "YES" : "NO");
    fflush(stdout);

    if (mode == 0 && fork() == 0) {
        sigpending(&pending);
        printf("[PID %d process] Is SIGUSR1 from parent pending (child): %s\n", getpid(), sigismember(&pending, SIGUSR1) ? "YES" : "NO");
    }
}


int main(int argc, char **argv) {
    if (argc != 3) {
        error("Two arguments expected");
    }
    int mode = -1;

    if (strcmp(argv[2], "fork") == 0) mode = 0;
    else if (strcmp(argv[2], "exec") == 0) mode = 1;
    else error("Wrong 2nd argument");

    if (strcmp(argv[1], "ignore") == 0) test_ignore(mode);
    else if (strcmp(argv[1], "handler") == 0) test_handler(mode);
    else if (strcmp(argv[1], "mask") == 0) test_mask(mode);
    else if (strcmp(argv[1], "pending") == 0) test_pending(mode);
    else error("Wrong 1st argument");

    if (mode == 1) {
        execl("./exec_test", "exec_test", argv[1], NULL);
    }

    return 0;
}

