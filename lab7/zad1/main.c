//
// Created by xraw on 12.05.2022.
//

#include "helper.h"

int pids[MAX_COOKS + MAX_DELIVERS];
int pid_counter = 0;

int sem_id, oven_shm_id, table_shm_id;
struct arr *oven;
struct arr *table;

void init_sem_set();

void init_shared_mem_segment();

void sigint_handler();

void print_semaphore_set(int id, int n);


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
    if ((sem_id = semget(get_key(SEM_SET_KEY_ID), 5, IPC_CREAT|IPC_EXCL|SEM_PERMISSION)) == -1) {
        error("Error while creating semaphore set");
    }

    union semun arg;

    arg.val = 1;
    if ((semctl(sem_id, OVEN_SEM, SETVAL, arg)) == -1) error("oven sem setval error");
    if ((semctl(sem_id, TABLE_SEM, SETVAL, arg)) == -1) error("table sem setval error");

    arg.val = ARR_SIZE;
    if ((semctl(sem_id, FULL_OVEN_SEM, SETVAL, arg)) == -1) error("full oven sem setval error");
    if ((semctl(sem_id, FULL_TABLE_SEM, SETVAL, arg)) == -1) error("full table sem setval error");

    arg.val = 0;
    if ((semctl(sem_id, EMPTY_TABLE_SEM, SETVAL, arg)) == -1) error("empty table sem setval error");

    print_semaphore_set(sem_id, 5);
}


void init_shared_mem_segment() {
    if((oven_shm_id = shmget(get_key(SHM_OVEN_KEY_ID), sizeof(struct arr), IPC_CREAT|IPC_EXCL|SHM_PERMISSION)) == -1) {
        error("Error while creating oven shared memory segment");
    }

    if((table_shm_id = shmget(get_key(SHM_TABLE_KEY_ID), sizeof(struct arr), IPC_CREAT|IPC_EXCL|SHM_PERMISSION)) == -1) {
        error("Error while creating table shared memory segment");
    }

    oven = shmat(oven_shm_id, NULL, 0);
    table = shmat(table_shm_id, NULL, 0);

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

    shmdt(oven);
    shmdt(table);
    semctl(sem_id, 0, IPC_RMID);
    shmctl(oven_shm_id, IPC_RMID, NULL);
    shmctl(table_shm_id, IPC_RMID, NULL);
    exit(EXIT_SUCCESS);
}


void print_semaphore_set(int id, int n) {
    int value = 0;
    printf("Semaphore %d values:\n", id);
    for (int i = 0; i < n; ++i) {
        if ((value = semctl(id, i, GETVAL)) == -1 ) {
            error("getval error");
        }
        printf("  %d: %d\n", i, value);
    }
}