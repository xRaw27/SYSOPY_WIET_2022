//
// Created by xraw on 12.05.2022.
//

#include "helper.h"

sem_t *oven_sem, *table_sem, *full_oven_sem, *full_table_sem, *empty_table_sem;
struct arr *table;

void add_to_arr(struct arr* arr, int n);

int remove_from_arr(struct arr* arr);


int main() {
    int pid = getpid();
    int n;
    char curr_time[15];

    int fd1, fd2;
    get_sem_addresses_and_shm_fds(&oven_sem, &table_sem, &full_oven_sem, &full_table_sem, &empty_table_sem, &fd1, &fd2);

    table = mmap(NULL, sizeof(struct arr), PROT_READ|PROT_WRITE, MAP_SHARED, fd2, 0);
    if (table == (void *) -1) error("Error while attaching table shared memory segment");

    srand(pid);
    printf("[deliverer] Startuje nowy dostawca z pid: %d\n", getpid());

    while (1) {
        // if table is empty blocks process
        sem_wait(empty_table_sem);

        // lock table
        sem_wait(table_sem);

        n = remove_from_arr(table);
        get_current_time(curr_time);
        printf("[deliverer | pid: %d | %s] Pobieram pizze: %d. Liczba pizz na stole: %d\n", pid, curr_time, n, table->count);

        sem_post(full_table_sem);

        // unlock table
        sem_post(table_sem);

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
    munmap(table, sizeof(struct arr));
    sem_close(oven_sem);
    sem_close(table_sem);
    sem_close(full_oven_sem);
    sem_close(full_table_sem);
    sem_close(empty_table_sem);
    exit(EXIT_SUCCESS);
}