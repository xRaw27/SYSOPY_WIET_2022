//
// Created by xraw on 04.04.2022.
//

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <signal.h>
#include <string.h>

int n;
int counter = 0;
int received_by_catcher = 0;

void error(char *message) {
    fprintf(stderr, "%s\n", message);
    exit(EXIT_FAILURE);
}

void set_sigaction(int signum, int flags, void *action) {
    struct sigaction act;

    sigemptyset(&act.sa_mask);
    act.sa_flags = flags;
    act.sa_sigaction = action;
    sigaction(signum, &act, NULL);
}

void sig1_action(int sig, siginfo_t *info, void *ucontext) {
    ++counter;
}

void sig2_action(int sig, siginfo_t *info, void *ucontext) {
    printf("[sender] Signals sent: %d, Signals received: %d\n", n, counter);
}

void send_kill(int catcher_pid, sigset_t *mask, int sig1, int sig2) {
    sigset_t sig1_mask;
    sigfillset(&sig1_mask);
    sigdelset(&sig1_mask, sig1);

    for(int i = 0; i < n; ++i) {
        kill(catcher_pid, sig1);
        sigsuspend(&sig1_mask);
    }
    kill(catcher_pid, sig2);
    sigsuspend(mask);
}

void send_sigqueue(int catcher_pid, sigset_t *mask, int sig1, int sig2) {
    sigset_t sig1_mask;
    sigfillset(&sig1_mask);
    sigdelset(&sig1_mask, sig1);

    union sigval si_val = {0};
    for(int i = 0; i < n; ++i) {
        sigqueue(catcher_pid, sig1, si_val);
        sigsuspend(&sig1_mask);
    }
    sigqueue(catcher_pid, sig2, si_val);
    sigsuspend(mask);
}

void set_mask(sigset_t *mask) {
    sigdelset(mask, SIGUSR1);
    sigdelset(mask, SIGUSR2);
    sigdelset(mask, SIGRTMIN);
    sigdelset(mask, SIGRTMIN + 1);
}

void set_catchers_and_send(int catcher_pid, sigset_t *mask, int sig1, int sig2, int queue) {
    set_sigaction(sig1, SA_SIGINFO, sig1_action);
    set_sigaction(sig2, SA_SIGINFO, sig2_action);
    if (queue) send_sigqueue(catcher_pid, mask, sig1, sig2);
    else send_kill(catcher_pid, mask, sig1, sig2);
}

int main(int argc, char **argv) {
    if (argc != 4) error("Three arguments expected");

    int catcher_pid = atoi(argv[1]);
    n = atoi(argv[2]);

    sigset_t mask;
    sigfillset(&mask);
    set_mask(&mask);

    if (strcmp(argv[3], "KILL") == 0) {
        set_catchers_and_send(catcher_pid, &mask, SIGUSR1, SIGUSR2, 0);
    }
    else if (strcmp(argv[3], "SIGQUEUE") == 0) {
        set_catchers_and_send(catcher_pid, &mask, SIGUSR1, SIGUSR2, 1);
    }
    else if (strcmp(argv[3], "SIGRT") == 0) {
        set_catchers_and_send(catcher_pid, &mask, SIGRTMIN, SIGRTMIN + 1, 0);
    }
    else error("Wrong mode argument");

    return 0;
}
