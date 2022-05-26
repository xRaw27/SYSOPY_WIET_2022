//
// Created by xraw on 12.05.2022.
//

#include "helper.h"

sem_t *oven_sem, *table_sem, *full_oven_sem, *full_table_sem, *empty_table_sem;
struct arr *oven;
struct arr *table;

void add_to_arr(struct arr* arr, int n);

int remove_from_arr(struct arr* arr);

void print_arr(struct arr* arr) {
    for (int i = 0; i < 5; ++i) {
        printf("%d ", arr->array[i]);
    }
    printf("\n");
}

int main() {
    int pid = getpid();
    int n;
    char curr_time[15];

    int fd1, fd2;
    get_sem_addresses_and_shm_fds(&oven_sem, &table_sem, &full_oven_sem, &full_table_sem, &empty_table_sem, &fd1, &fd2);

    oven = mmap(NULL, sizeof(struct arr), PROT_READ|PROT_WRITE, MAP_SHARED, fd1, 0);
    if (oven == (void *) -1) error("Error while attaching oven shared memory segment");

    table = mmap(NULL, sizeof(struct arr), PROT_READ|PROT_WRITE, MAP_SHARED, fd2, 0);
    if (table == (void *) -1) error("Error while attaching table shared memory segment");

    srand(pid);
    printf("[cook] Startuje nowy kucharz z pid: %d.\n", pid);

    while (1) {
        n = (int)rand_number(0, 9);
        get_current_time(curr_time);
        printf("[cook | pid: %d | %s] Przygotowuje pizze: %d.\n", pid, curr_time, n);
        milliseconds_sleep(rand_number(1000, 2000));

        // if oven is full blocks process
        sem_wait(full_oven_sem);

        // lock oven
        sem_wait(oven_sem);

        add_to_arr(oven, n);
        get_current_time(curr_time);
        printf("[cook | pid: %d | %s] Dodałem pizze: %d. Liczba pizz w piecu: %d.\n", pid, curr_time, n, oven->count);

        // unlock oven
        sem_post(oven_sem);

        milliseconds_sleep(rand_number(4000, 5000));

        // removing from oven
        sem_wait(oven_sem);

        n = remove_from_arr(oven);
        get_current_time(curr_time);
        printf("[cook | pid: %d | %s] Wyjmuję pizze: %d. Liczba pizz w piecu: %d.\n", pid, curr_time, n, oven->count);
        sem_post(oven_sem);

        // new empty place in oven
        sem_post(full_oven_sem);

        // add to table
        sem_wait(full_table_sem);
        sem_wait(table_sem);

        add_to_arr(table, n);
        printf("[cook | pid: %d | %s] Umieszczono na stole pizzę: %d. Liczba pizz na stole: %d.\n", pid, curr_time, n, table->count);
        sem_post(empty_table_sem);
        sem_post(table_sem);
    }
}


void add_to_arr(struct arr* arr, int n) {
    arr->array[arr->last] = n;
    arr->last = ((arr->last + 1) % ARR_SIZE);
    ++arr->count;
}


int remove_from_arr(struct arr* arr) {
    int n = arr->array[arr->first];
    arr->first = ((arr->first + 1) % ARR_SIZE);
    --arr->count;
    return n;
}


void sigint_handler() {
    munmap(oven, sizeof(struct arr));
    munmap(table, sizeof(struct arr));
    sem_close(oven_sem);
    sem_close(table_sem);
    sem_close(full_oven_sem);
    sem_close(full_table_sem);
    sem_close(empty_table_sem);
    exit(EXIT_SUCCESS);
}
