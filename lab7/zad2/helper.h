//
// Created by xraw on 12.05.2022.
//

#ifndef HELPER_H
#define HELPER_H

#define SEM_PERMISSION 0660
#define SHM_PERMISSION 0660


#define OVEN_SEM_NAME "/oven_sem"
#define TABLE_SEM_NAME "/table_sem"
#define FULL_OVEN_SEM_NAME "/full_oven_sem"
#define FULL_TABLE_SEM_NAME "/full_table_sem"
#define EMPTY_TABLE_SEM_NAME "/empty_table_sem"

#define SHM_OVEN_NAME "/shm_oven"
#define SHM_TABLE_NAME "/shm_table"

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
#include <signal.h>

#include <semaphore.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>


struct arr {
    int array[ARR_SIZE];
    int count;
    int first;
    int last;
};


void error(char *message) {
    if (errno != 0) {
        perror(message);
    } else {
        fprintf(stderr, "%s\n", message);
    }
    exit(EXIT_FAILURE);
}


void get_sem_addresses_and_shm_fds(sem_t **oven_sem, sem_t **table_sem, sem_t **full_oven_sem, sem_t **full_table_sem, sem_t **empty_table_sem, int *fd_oven, int *fd_table) {
    if ((*oven_sem = sem_open(OVEN_SEM_NAME, O_RDWR)) == SEM_FAILED) {
        error("Error while creating oven semaphore");
    }
    if ((*table_sem = sem_open(TABLE_SEM_NAME, O_RDWR)) == SEM_FAILED) {
        error("Error while creating table semaphore");
    }
    if ((*full_oven_sem = sem_open(FULL_OVEN_SEM_NAME, O_RDWR)) == SEM_FAILED) {
        error("Error while creating full oven semaphore");
    }
    if ((*full_table_sem = sem_open(FULL_TABLE_SEM_NAME, O_RDWR)) == SEM_FAILED) {
        error("Error while creating full table semaphore");
    }
    if ((*empty_table_sem = sem_open(EMPTY_TABLE_SEM_NAME, O_RDWR)) == SEM_FAILED) {
        error("Error while creating empty table semaphore");
    }

    if ((*fd_oven = shm_open(SHM_OVEN_NAME, O_RDWR, SHM_PERMISSION))== -1) {
        error("Error while creating oven shared memory segment");
    }
    if ((*fd_table = shm_open(SHM_TABLE_NAME, O_RDWR, SHM_PERMISSION)) == -1) {
        error("Error while creating table shared memory segment");
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
