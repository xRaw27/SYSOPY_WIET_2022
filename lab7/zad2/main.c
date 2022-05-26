//
// Created by xraw on 12.05.2022.
//

#include "helper.h"

int pids[MAX_COOKS + MAX_DELIVERS];
int pid_counter = 0;

sem_t *oven_sem, *table_sem, *full_oven_sem, *full_table_sem, *empty_table_sem;
struct arr *oven;
struct arr *table;

void init_sem_set();

void init_shared_mem_segment();

void sigint_handler();


int main(int argc, char **argv) {
    if (argc != 3) {
        error("Two arguments expected");
    }

    int n = atoi(argv[1]);
    int m = atoi(argv[2]);
    if (n < 0 || n > MAX_COOKS) error("Wrong number of cooks");
    if (m < 0 || m > MAX_DELIVERS) error("Wrong number of deliverers");

    init_sem_set();
    init_shared_mem_segment();
    signal(SIGINT, sigint_handler);

    pid_t pid;
    for (int i = 0; i < n + m; ++i) {
        pid = fork();
        if (pid == 0) {
            if (i < n) execl("./cook", "./cook", NULL);
            else execl("./deliverer", "./deliverer", NULL);
        }
        else {
            pids[pid_counter] = pid;
            ++pid_counter;
        }
    }

    while (1);
}


void init_sem_set() {
    if ((oven_sem = sem_open(OVEN_SEM_NAME, O_CREAT|O_EXCL|O_RDWR, SEM_PERMISSION, 1)) == SEM_FAILED) {
        error("Error while creating oven semaphore");
    }
    if ((table_sem = sem_open(TABLE_SEM_NAME, O_CREAT|O_EXCL|O_RDWR, SEM_PERMISSION, 1)) == SEM_FAILED) {
        error("Error while creating table semaphore");
    }
    if ((full_oven_sem = sem_open(FULL_OVEN_SEM_NAME, O_CREAT|O_EXCL|O_RDWR, SEM_PERMISSION, ARR_SIZE)) == SEM_FAILED) {
        error("Error while creating full oven semaphore");
    }
    if ((full_table_sem = sem_open(FULL_TABLE_SEM_NAME, O_CREAT|O_EXCL|O_RDWR, SEM_PERMISSION, ARR_SIZE)) == SEM_FAILED) {
        error("Error while creating full table semaphore");
    }
    if ((empty_table_sem = sem_open(EMPTY_TABLE_SEM_NAME, O_CREAT|O_EXCL|O_RDWR, SEM_PERMISSION, 0)) == SEM_FAILED) {
        error("Error while creating empty table semaphore");
    }
}


void init_shared_mem_segment() {
    int fd1, fd2;

    if ((fd1 = shm_open(SHM_OVEN_NAME, O_CREAT|O_EXCL|O_RDWR, SHM_PERMISSION))== -1) {
        error("Error while creating oven shared memory segment");
    }
    if ((fd2 = shm_open(SHM_TABLE_NAME, O_CREAT|O_EXCL|O_RDWR, SHM_PERMISSION)) == -1) {
        error("Error while creating table shared memory segment");
    }

    if (ftruncate(fd1, sizeof(struct arr)) == -1) {
        error("Oven shared memory ftruncate error");
    }

    if (ftruncate(fd2, sizeof(struct arr)) == -1) {
        error("Table shared memory ftruncate error");
    }

    oven = mmap(NULL, sizeof(struct arr), PROT_READ|PROT_WRITE, MAP_SHARED, fd1, 0);
    if (oven == (void *) -1) error("Error while attaching oven shared memory segment");

    table = mmap(NULL, sizeof(struct arr), PROT_READ|PROT_WRITE, MAP_SHARED, fd2, 0);
    if (table == (void *) -1) error("Error while attaching table shared memory segment");

    oven->first = 0;
    oven->last = 0;
    oven->count = 0;
    table->first = 0;
    table->last = 0;
    table->count = 0;
}


void sigint_handler() {
    for (int i = 0; i < pid_counter; ++i) {
        kill(pids[i], SIGINT);
    }

    munmap(oven, sizeof(struct arr));
    munmap(table, sizeof(struct arr));
    sem_close(oven_sem);
    sem_close(table_sem);
    sem_close(full_oven_sem);
    sem_close(full_table_sem);
    sem_close(empty_table_sem);
    sem_unlink(OVEN_SEM_NAME);
    sem_unlink(TABLE_SEM_NAME);
    sem_unlink(FULL_OVEN_SEM_NAME);
    sem_unlink(FULL_TABLE_SEM_NAME);
    sem_unlink(EMPTY_TABLE_SEM_NAME);
    shm_unlink(SHM_OVEN_NAME);
    shm_unlink(SHM_TABLE_NAME);
    exit(EXIT_SUCCESS);
}

