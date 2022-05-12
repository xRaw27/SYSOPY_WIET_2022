//
// Created by xraw on 12.05.2022.
//

#ifndef HELPER_H
#define HELPER_H

#define SEM_PERMISSION 0660
#define SHM_PERMISSION 0660

#define SEM_SET_KEY_ID 1
#define SHM_OVEN_KEY_ID 2
#define SHM_TABLE_KEY_ID 3

#define OVEN_SEM 0
#define TABLE_SEM 1
#define FULL_OVEN_SEM 2
#define FULL_TABLE_SEM 3
#define EMPTY_TABLE_SEM 4

#define ARR_SIZE 5
#define MAX_COOKS 1000
#define MAX_DELIVERS 1000

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <sys/time.h>
#include <string.h>
#include <errno.h>
#include <sys/sem.h>
#include <sys/shm.h>
#include <sys/ipc.h>
#include <sys/types.h>
#include <signal.h>


struct arr {
    int array[ARR_SIZE];
    int count;
    int first;
    int last;
};


union semun {
    int val;
    struct semid_ds *buf;
    unsigned short *array;
    struct seminfo *__buf;
};


void error(char *message) {
    if (errno != 0) {
        perror(message);
    } else {
        fprintf(stderr, "%s\n", message);
    }
    exit(EXIT_FAILURE);
}


key_t get_key(int proj_id) {
    const char *home = getenv("HOME");
    if (!home) error("getenv error");

    key_t key;
    if ((key = ftok(home, proj_id)) == -1) error("Error while generating key");

    return key;
}


void sem_decrement(int sem_id, int sem_num) {
    struct sembuf buffer = {sem_num, -1, 0};
    if (semop(sem_id, &buffer, 1) == -1) error("Semaphore decrement error");
}


void sem_increment(int sem_id, int sem_num) {
    struct sembuf buffer = {sem_num, 1, 0};
    if (semop(sem_id, &buffer, 1) == -1) error("Semaphore increment error");
}


void get_sem_and_shm_ids(int *sem_id, int *oven_shm_id, int *table_shm_id) {
    if ((*sem_id = semget(get_key(SEM_SET_KEY_ID), 0, 0)) == -1) {
        error("semget error");
    }
    if((*oven_shm_id = shmget(get_key(SHM_OVEN_KEY_ID), 0, 0)) == -1) {
        error("Oven shmget error");
    }
    if((*table_shm_id = shmget(get_key(SHM_TABLE_KEY_ID), 0, 0)) == -1) {
        error("Table shmget error");
    }
}


void milliseconds_sleep(unsigned int t) {
    struct timespec ts;

    ts.tv_sec = t / 1000;
    ts.tv_nsec = (t % 1000) * 1000000;

    if (nanosleep(&ts, &ts) == -1) {
        error("Error while sleeping");
    }
}


unsigned int rand_number(unsigned int min, unsigned int max) {
    return (unsigned int)((double)random()/RAND_MAX * (max - min + 1) + min);
}


void get_current_time(char *curr_time) {
    struct timeval t;
    char buffer[10];

    gettimeofday(&t, NULL);
    strftime(buffer, 20, "%H:%M:%S", localtime(&t.tv_sec));
    sprintf(curr_time, "%s:%03ld", buffer, t.tv_usec / 1000);
}


#endif //HELPER_H
