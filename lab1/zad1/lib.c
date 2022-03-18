//
// Created by xraw on 15.03.2022.
//

#include "lib.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char **table = NULL;
size_t table_size = 0;

void create_table(size_t size) {
    if (table) {
        printf("[create_table] Table already exists\n");
        return;
    }

    table = calloc(size, sizeof(char *));
    table_size = size;
    printf("[create_table] Created table of size %lu\n", table_size);
}

int is_table_created() {
    if (!table) {
        printf("[is_table_created] Table has not been created\n");
        return 0;
    }
    return 1;
}

void wc_to_temp(int files_count, char **file_names) {
    if (files_count < 1) {
        printf("[wc_to_temp] No files given\n");
        return;
    }

    size_t buffer_size = 10;
    for (int i = 0; i < files_count; i++) {
        buffer_size += strlen(file_names[i]) + 1;
    }

    char *buffer = calloc(buffer_size, sizeof(char));

    strcat(buffer, "wc");
    for (int i = 0; i < files_count; i++) {
        strcat(buffer, " ");
        strcat(buffer, file_names[i]);
    }
    strcat(buffer, " > temp");
    system(buffer);
    printf("[wc_to_temp] Result of wc for %d file(s) saved to temp\n", files_count);

    free(buffer);
}

size_t find_first_empty_block() {
    if (!is_table_created()) {
        return table_size;
    }

    for (size_t i = 0; i < table_size; i++) {
        if (!table[i]) {
            return i;
        }
    }

    return table_size;
}

size_t save_block(int no_print) {
    size_t block_index = find_first_empty_block();

    if (block_index == table_size) {
        printf("[save_block] Empty block not found\n");
        return table_size;
    }

    FILE *fptr;
    fptr = fopen("temp", "r");

    fseek(fptr, 0, SEEK_END);
    size_t block_size = ftell(fptr);
    fseek(fptr, 0, SEEK_SET);

    table[block_index] = calloc(block_size, sizeof(char));

    fread(table[block_index], block_size, 1, fptr);
    table[block_index][block_size - 1] = '\0';
    fclose(fptr);

    if (!no_print) {
        printf("[save_block] New block size: %ld \n", block_size);
        printf("[save_block] Saved block at index: %lu \n", block_index);
    }

    return block_index;
}

void delete_block(size_t index) {
    if (!is_table_created()) {
        return;
    }
    if (index >= table_size) {
        printf("[delete_block] Given index is greater than size of table\n");
        return;
    }

    if (table[index]) {
        free(table[index]);
        table[index] = NULL;
    }
}

void free_table() {
    for (size_t i = 0; i < table_size; i++) {
        if (table[i]) {
            free(table[i]);
        }
    }
    free(table);
}

void print_table() {
    printf("[print_blocks] Blocks table:\n\n");

    for (int i = 0; i < table_size; i++) {
        printf("%s \n\n", table[i]);
    }
}
