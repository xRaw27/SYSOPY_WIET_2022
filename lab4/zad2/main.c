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
#include <sys/wait.h>

int counter;


void error(char *message) {
    fprintf(stderr, "%s\n", message);
    exit(EXIT_FAILURE);
}

void set_sigaction(int signum, int flags, void *handler, void *action) {
    struct sigaction act;

    sigemptyset(&act.sa_mask);
    act.sa_flags = flags;
    if (action == NULL) act.sa_handler = handler;
    else act.sa_sigaction = action;
    sigaction(signum, &act, NULL);
}


void signal_action1(int sig, siginfo_t *info, void *ucontext) {
    printf("Signal: %d\n", sig);
    printf("  si_code: %d\n", info->si_code);
    printf("  si_pid: %d\n", info->si_pid);
    printf("  si_uid: %d\n", info->si_uid);
    printf("  si_value: %d\n", info->si_value.sival_int);
}

void test1() {
    set_sigaction(SIGUSR1, SA_SIGINFO, NULL, signal_action1);

    printf("\n========== Test 1 - SA_SIGINFO flag test ==========\n");

    printf("\n>>> sigqueue with si_val 12345 <<<\n"); fflush(stdout);
    union sigval si_val = {12345};
    sigqueue(getpid(), SIGUSR1, si_val);

    printf("\n>>> raise <<<\n"); fflush(stdout);
    raise(SIGUSR1);

    printf("\n>>> sigkill in child process <<<\n"); fflush(stdout);
    if (fork() == 0) {
        kill(getppid(), SIGUSR1);
        exit(EXIT_SUCCESS);
    } else {
        wait(NULL);
    }
}


void signal_handler2(int sig) {
    int id = counter;
    printf("Signal %d start, counter: %d\n", sig, id);
    ++counter;
    if (counter < 5) {
        raise(SIGUSR1);
    }
    printf("Signal %d end, counter: %d\n", sig, id);
    fflush(stdout);
}

void test2() {
    printf("\n========== Test 2 - SA_NODEFER flag test ==========\n");

    printf("\n>>> SA_NODEFER flag not set <<<\n");
    counter = 0;
    set_sigaction(SIGUSR1, 0, signal_handler2, NULL);
    raise(SIGUSR1);

    printf("\n>>> SA_NODEFER flag set <<<\n");
    counter = 0;
    set_sigaction(SIGUSR1, SA_NODEFER, signal_handler2, NULL);
    raise(SIGUSR1);
}


void signal_handler3(int sig) {
    printf("Signal SIGCHLD handler, received signal: %d\n", sig);
}

void test3() {
    printf("\n========== Test 3 - SA_NOCLDSTOP flag test (handler for SIGCHLD signal) ==========\n");
    set_sigaction(SIGCHLD, 0, signal_handler3, NULL);

    printf("\n>>> SA_NOCLDSTOP flag not set, child exit success <<<\nwaiting 1 second for signal...\n"); fflush(stdout);
    if (fork() == 0) exit(EXIT_SUCCESS);
    sleep(1);
    printf("waiting ended\n");

    printf("\n>>> SA_NOCLDSTOP flag not set, child receive sigstop <<<\nwaiting 1 second for signal...\n"); fflush(stdout);
    if (fork() == 0) raise(SIGSTOP);
    sleep(1);
    printf("waiting ended\n");

    set_sigaction(SIGCHLD, SA_NOCLDSTOP, signal_handler3, NULL);

    printf("\n>>> SA_NOCLDSTOP flag set, child exit success <<<\nwaiting 1 second for signal...\n"); fflush(stdout);
    if (fork() == 0) exit(EXIT_SUCCESS);
    sleep(1);
    printf("waiting ended\n");

    printf("\n>>> SA_NOCLDSTOP flag set, child receive sigstop <<<\nwaiting 1 second for signal...\n"); fflush(stdout);
    if (fork() == 0) raise(SIGSTOP);
    sleep(1);
    printf("waiting ended\n");
}

int main() {
    test1();
    test2();
    test3();

    return 0;
}

