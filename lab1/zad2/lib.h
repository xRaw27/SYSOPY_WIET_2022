//
// Created by xraw on 15.03.2022.
//

#ifndef SYSOPY_LIB_H
#define SYSOPY_LIB_H
#include <stddef.h>

void create_table(size_t size);
int is_table_created();
void wc_to_temp(int files_count, char **file_names);
size_t find_first_empty_block();
size_t save_block(int no_print);
void delete_block(size_t index);
void free_table();
void print_table();

#endif //SYSOPY_LIB_H
