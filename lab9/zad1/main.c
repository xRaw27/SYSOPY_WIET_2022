//
// Created by xraw on 02.06.2022.
//

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <pthread.h>
#include <sys/time.h>
#include <time.h>

#define ELVES_NUMBER 10
#define MAX_ELVES_WAITING 3
#define REINDEER_NUMBER 9

int reindeers_back = 0;
int end_of_delivery = 0;

int elves_waiting = 0;
int waiting_elf_ids[MAX_ELVES_WAITING];

struct args {
    int id;
};

struct args *id_args;

pthread_mutex_t santa_sleep_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t santa_sleep_cond = PTHREAD_COND_INITIALIZER;

pthread_mutex_t reindeer_back_mutex = PTHREAD_MUTEX_INITIALIZER;

pthread_mutex_t reindeer_delivery_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t reindeer_delivery_cond = PTHREAD_COND_INITIALIZER;

pthread_mutex_t elf_report_problem_mutex = PTHREAD_MUTEX_INITIALIZER;

pthread_mutex_t elf_full_queue_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t elf_full_queue_cond = PTHREAD_COND_INITIALIZER;

pthread_mutex_t elf_solving_problem_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t elf_solving_problem_cond = PTHREAD_COND_INITIALIZER;


void *santa_claus(void *arg);

void *elf(void *arg);

void *reindeer(void *arg);

void error(char *message);

void sleep_min_max(unsigned int min, unsigned int max);

void free_args();


int main() {
    srand(getpid());

    id_args = calloc(1 + REINDEER_NUMBER + ELVES_NUMBER, sizeof(struct args));
    atexit(free_args);

    int id = 0;

    pthread_t santa_claus_thread_id;
    id_args[id].id = id;
    pthread_create(&santa_claus_thread_id, NULL, &santa_claus, &id_args[id++]);

    pthread_t *reindeer_thread_ids = calloc(REINDEER_NUMBER, sizeof(pthread_t));
    for (int i = 0; i < REINDEER_NUMBER; ++i) {
        id_args[id].id = id;
        pthread_create(&reindeer_thread_ids[i], NULL, &reindeer, &id_args[id++]);
    }

    pthread_t *elf_thread_ids = calloc(ELVES_NUMBER, sizeof(pthread_t));
    for (int i = 0; i < ELVES_NUMBER; ++i) {
        id_args[id].id = id;
        pthread_create(&elf_thread_ids[i], NULL, &elf, &id_args[id++]);
    }


    pthread_join(santa_claus_thread_id, NULL);
    for (int i = 0; i < REINDEER_NUMBER; ++i) {
        pthread_join(reindeer_thread_ids[i], NULL);
    }
    for (int i = 0; i < ELVES_NUMBER; ++i) {
        pthread_join(elf_thread_ids[i], NULL);
    }

    return 0;
}


void *santa_claus(void *arg) {
    struct args *args = arg;
    int deliveries_completed = 0;

    printf("[reindeer] Santa claus starts, ID = %d\n", args->id);

    while (deliveries_completed < 3) {
        pthread_mutex_lock(&santa_sleep_mutex);
        while (elves_waiting < MAX_ELVES_WAITING && reindeers_back < REINDEER_NUMBER) {
            printf("[santa_claus] I am sleeping\n");
            pthread_cond_wait(&santa_sleep_cond, &santa_sleep_mutex);
        }
        pthread_mutex_unlock(&santa_sleep_mutex);

        printf("[santa_claus] I am waking up\n");

        pthread_mutex_lock(&reindeer_back_mutex);
        if (reindeers_back == REINDEER_NUMBER) {
            printf("[santa_claus] Delivering toys\n");
            sleep_min_max(2000, 4000);

            reindeers_back = 0;

            pthread_mutex_lock(&reindeer_delivery_mutex);
            end_of_delivery = 1;
            pthread_cond_broadcast(&reindeer_delivery_cond);
            pthread_mutex_unlock(&reindeer_delivery_mutex);

            ++deliveries_completed;
        }
        pthread_mutex_unlock(&reindeer_back_mutex);

        pthread_mutex_lock(&elf_report_problem_mutex);
        if (elves_waiting == MAX_ELVES_WAITING) {
            printf("[santa_claus] Solving elves problems, Elf ids = %d, %d, %d\n", waiting_elf_ids[0], waiting_elf_ids[1], waiting_elf_ids[2]);

            sleep_min_max(1000, 2000);

            elves_waiting = 0;

            pthread_mutex_lock(&elf_solving_problem_mutex);
            pthread_cond_broadcast(&elf_solving_problem_cond);
            pthread_mutex_unlock(&elf_solving_problem_mutex);

            pthread_mutex_lock(&elf_full_queue_mutex);
            pthread_cond_broadcast(&elf_full_queue_cond);
            pthread_mutex_unlock(&elf_full_queue_mutex);
        }
        pthread_mutex_unlock(&elf_report_problem_mutex);

        printf("[santa_claus] Santa goes back to sleep\n");
    }

    exit(0);
}

void *elf(void *arg) {
    struct args *args = arg;

    printf("[elf] Elf starts, ID = %d\n", args->id);

    while (1) {
        sleep_min_max(2000, 5000);

        pthread_mutex_lock(&elf_full_queue_mutex);
        while (elves_waiting == MAX_ELVES_WAITING) {
            printf("[elf] Elf waits for other elves to return, ID = %d\n", args->id);
            pthread_cond_wait(&elf_full_queue_cond, &elf_full_queue_mutex);
        }
        pthread_mutex_unlock(&elf_full_queue_mutex);

        pthread_mutex_lock(&elf_report_problem_mutex);
        if (elves_waiting < MAX_ELVES_WAITING) {
            waiting_elf_ids[elves_waiting++] = args->id;

            if (elves_waiting == MAX_ELVES_WAITING) {
                printf("[elf] Waking up Santa, ID = %d\n", args->id);

                pthread_mutex_lock(&santa_sleep_mutex);
                pthread_cond_broadcast(&santa_sleep_cond);
                pthread_mutex_unlock(&santa_sleep_mutex);
            }
        }
        pthread_mutex_unlock(&elf_report_problem_mutex);

        pthread_mutex_lock(&elf_solving_problem_mutex);
        while (elves_waiting > 0) {
            pthread_cond_wait(&elf_solving_problem_cond, &elf_solving_problem_mutex);
        }
        pthread_mutex_unlock(&elf_solving_problem_mutex);
    }
}


void *reindeer(void *arg) {
    struct args *args = arg;

    printf("[reindeer] Reindeer starts, ID = %d\n", args->id);

    while (1) {
        sleep_min_max(5000, 10000);

        pthread_mutex_lock(&reindeer_back_mutex);
        end_of_delivery = 0;
        ++reindeers_back;
        printf("[reindeer] %d reindeers are waiting for Santa, ID = %d\n", reindeers_back, args->id);

        if (reindeers_back == REINDEER_NUMBER) {
            printf("[reindeer] Waking up Santa, ID = %d\n", args->id);

            pthread_mutex_lock(&santa_sleep_mutex);
            pthread_cond_broadcast(&santa_sleep_cond);
            pthread_mutex_unlock(&santa_sleep_mutex);
        }
        pthread_mutex_unlock(&reindeer_back_mutex);

        pthread_mutex_lock(&reindeer_delivery_mutex);
        while (!end_of_delivery) {
            pthread_cond_wait(&reindeer_delivery_cond, &reindeer_delivery_mutex);
        }
        pthread_mutex_unlock(&reindeer_delivery_mutex);
    }
}


void sleep_min_max(unsigned int min, unsigned int max) {
    unsigned int milliseconds = (unsigned int)((double)random()/RAND_MAX * (max - min + 1) + min);
    struct timespec ts;

    ts.tv_sec = milliseconds / 1000;
    ts.tv_nsec = (milliseconds % 1000) * 1000000;
    if (nanosleep(&ts, &ts) == -1) {
        error("Error while sleeping");
    }
}


void free_args() {
    free(id_args);
}


void error(char *message) {
    if (errno != 0) {
        perror(message);
    }
    else {
        fprintf(stderr, "[error] %s\n", message);
    }
    exit(EXIT_FAILURE);
}
