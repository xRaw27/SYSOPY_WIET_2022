//
// Created by xraw on 12.05.2022.
//

#include "helper.h"

struct arr *oven;
struct arr *table;

void add_to_arr(struct arr* arr, int n);

int remove_from_arr(struct arr* arr);

//void print_arr(struct arr* arr) {
//    for (int i = 0; i < 5; ++i) {
//        printf("%d ", arr->array[i]);
//    }
//    printf("\n");
//}

int main() {
    int pid = getpid();
    int n;
    char curr_time[15];

    int sem_id, oven_shm_id, table_shm_id;
    get_sem_and_shm_ids(&sem_id, &oven_shm_id, &table_shm_id);

    oven = shmat(oven_shm_id, NULL, 0);
    table = shmat(table_shm_id, NULL, 0);

    srand(pid);
    printf("[cook] Startuje nowy kucharz z pid: %d.\n", pid);

    while (1) {
        n = (int)rand_number(0, 9);
        get_current_time(curr_time);
        printf("[cook | pid: %d | %s] Przygotowuje pizze: %d.\n", pid, curr_time, n);
        milliseconds_sleep(rand_number(1000, 2000));

        // if oven is full blocks process
        sem_decrement(sem_id, FULL_OVEN_SEM);

        // lock oven
        sem_decrement(sem_id, OVEN_SEM);

        add_to_arr(oven, n);
        get_current_time(curr_time);
        printf("[cook | pid: %d | %s] Dodałem pizze: %d. Liczba pizz w piecu: %d.\n", pid, curr_time, n, oven->count);

        // unlock oven
        sem_increment(sem_id, OVEN_SEM);

        milliseconds_sleep(rand_number(4000, 5000));

        // removing from oven
        sem_decrement(sem_id, OVEN_SEM);
        n = remove_from_arr(oven);
        get_current_time(curr_time);
        printf("[cook | pid: %d | %s] Wyjmuję pizze: %d. Liczba pizz w piecu: %d.\n", pid, curr_time, n, oven->count);
        sem_increment(sem_id, OVEN_SEM);

        // new empty place in oven
        sem_increment(sem_id, FULL_OVEN_SEM);

        // add to table
        sem_decrement(sem_id, FULL_TABLE_SEM);
        sem_decrement(sem_id, TABLE_SEM);
        add_to_arr(table, n);
        printf("[cook | pid: %d | %s] Umieszczono na stole pizzę: %d. Liczba pizz na stole: %d.\n", pid, curr_time, n, table->count);
        sem_increment(sem_id, EMPTY_TABLE_SEM);
        sem_increment(sem_id, TABLE_SEM);
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
    shmdt(oven);
    shmdt(table);
    exit(EXIT_SUCCESS);
}
