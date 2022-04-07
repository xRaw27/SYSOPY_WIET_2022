//
// Created by xraw on 04.04.2022.
//

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <signal.h>
#include <string.h>

sigset_t mask;
int counter = 0;
int sender_pid = -2;

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

void sig1_action(int sig1, siginfo_t *info, void *ucontext) {
    sender_pid = info->si_pid;
    ++counter;

    if (info->si_code == SI_USER) {
        kill(sender_pid, sig1);
    }
    else {
        union sigval si_val = {counter};
        sigqueue(sender_pid, sig1, si_val);
    }

    sigsuspend(&mask);
}

void sig2_action(int sig2, siginfo_t *info, void *ucontext) {
    if (info->si_code == SI_USER) {
        kill(sender_pid, sig2);
    }
    else {
        union sigval si_val = {counter};
        sigqueue(sender_pid, sig2, si_val);
    }

    printf("[catcher] Signals received: %d\n", counter);
}

void set_mask_and_catchers() {
    sigdelset(&mask, SIGUSR1);
    sigdelset(&mask, SIGUSR2);
    sigdelset(&mask, SIGRTMIN);
    sigdelset(&mask, SIGRTMIN + 1);

    set_sigaction(SIGUSR1, SA_SIGINFO, sig1_action);
    set_sigaction(SIGUSR2, SA_SIGINFO, sig2_action);
    set_sigaction(SIGRTMIN, SA_SIGINFO, sig1_action);
    set_sigaction(SIGRTMIN + 1, SA_SIGINFO, sig2_action);
}

int main(int argc, char **argv) {
    printf("[catcher] PID: %d\n", getpid());
    fflush(stdout);

    sigfillset(&mask);
    set_mask_and_catchers();

    sigsuspend(&mask);
}
