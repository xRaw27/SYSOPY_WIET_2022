//
// Created by xraw on 04.04.2022.
//

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <string.h>

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

void sig1_action(int sig, siginfo_t *info, void *ucontext) {
    sender_pid = info->si_pid;
    ++counter;
}

void sig2_action(int sig2, siginfo_t *info, void *ucontext) {
    int sig1;
    if (sig2 == SIGUSR2) sig1 = SIGUSR1;
    else sig1 = SIGRTMIN;

    if (info->si_code == SI_USER) {
        for (int i = 0; i < counter; ++i) {
            kill(sender_pid, sig1);
        }
        kill(sender_pid, sig2);
    }
    else {
        union sigval si_val;
        for (int i = 0; i < counter; ++i) {
            si_val.sival_int = i + 1;
            sigqueue(sender_pid, sig1, si_val);
        }
        si_val.sival_int = counter;
        sigqueue(sender_pid, sig2, si_val);
    }

    printf("[catcher] Signals received: %d\n", counter);
    exit(EXIT_SUCCESS);
}

void set_mask_and_catchers(sigset_t *mask) {
    sigdelset(mask, SIGUSR1);
    sigdelset(mask, SIGUSR2);
    sigdelset(mask, SIGRTMIN);
    sigdelset(mask, SIGRTMIN + 1);

    set_sigaction(SIGUSR1, SA_SIGINFO, sig1_action);
    set_sigaction(SIGUSR2, SA_SIGINFO, sig2_action);
    set_sigaction(SIGRTMIN, SA_SIGINFO, sig1_action);
    set_sigaction(SIGRTMIN + 1, SA_SIGINFO, sig2_action);
}

int main(int argc, char **argv) {
    printf("[catcher] PID: %d\n", getpid());
    fflush(stdout);

    sigset_t mask;
    sigfillset(&mask);
    set_mask_and_catchers(&mask);

    while (1) {
        sigsuspend(&mask);
    }
}
