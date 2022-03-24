#ifndef _MLOCK_H_
#define _MLOCK_H_
#include <stdbool.h>
void configure_malloc_behavior(void);
void reserve_process_memory(int size);
bool check_pagefault(int allowed_maj, int allowed_min);

#endif