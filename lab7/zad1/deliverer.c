//
// Created by xraw on 12.05.2022.
//

#include "helper.h"

struct arr *table;

void add_to_arr(struct arr* arr, int n);

int remove_from_arr(struct arr* arr);


int main() {
    int pid = getpid();
    int n;
    char curr_time[15];

    int sem_id, oven_shm_id, table_shm_id;
    get_sem_and_shm_ids(&sem_id, &oven_shm_id, &table_shm_id);

    table = shmat(table_shm_id, NULL, 0);

    srand(pid);
    printf("[deliverer] Startuje nowy dostawca z pid: %d\n", getpid());

    while (1) {
        // if table is empty blocks process
        sem_decrement(sem_id, EMPTY_TABLE_SEM);

        // lock table
        sem_decrement(sem_id, TABLE_SEM);

        n = remove_from_arr(table);
        get_current_time(curr_time);
        printf("[deliverer | pid: %d | %s] Pobieram pizze: %d. Liczba pizz na stole: %d\n", pid, curr_time, n, table->count);

        sem_increment(sem_id, FULL_TABLE_SEM);

        // unlock table
        sem_increment(sem_id, TABLE_SEM);

        milliseconds_sleep(rand_number(4000, 5000));

        get_current_time(curr_time);
        printf("[deliverer | pid: %d | %s] Dostarczam pizze: %u.\n", pid, curr_time, n);
        milliseconds_sleep(rand_number(4000, 5000));
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
    shmdt(table);
    exit(EXIT_SUCCESS);
}