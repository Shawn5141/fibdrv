#include <assert.h>
#include <fcntl.h>
#include <limits.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <time.h>
#include <unistd.h>

#include "mlock.h"

#define FIB_DEV "/dev/fibonacci"
#define KTIME_ENABLE "/sys/class/fibonacci/fibonacci/ktime_measure"
#define FIB_METHOD "/sys/class/fibonacci/fibonacci/fib_method"

#define PRE_ALLOCATION_SIZE \
    (64 * 1024 * 1024)  // ulimit -l to check the maxinum size may lock into
                        // memory in process


static inline long long elapsed(struct timespec *t1, struct timespec *t2)
{
    return (long long) (t2->tv_sec - t1->tv_sec) * 1e9 +
           (long long) (t2->tv_nsec - t1->tv_nsec);
}

int main()
{
    // mlock
    configure_malloc_behavior();
    /* malloc and touch generated */
    reserve_process_memory(PRE_ALLOCATION_SIZE);
    check_pagefault(INT_MAX, INT_MAX);
    /* 2nd malloc and use gnenrated */
    reserve_process_memory(PRE_ALLOCATION_SIZE);
    assert(check_pagefault(0, 0));
    int err = 0;
    long long sz;
    char buf[1];
    char write_buf[] = "testing writing";
    int offset = 100; /* TODO: try test something bigger than the limit */
    struct timespec t1, t2;

    int fd = open(FIB_DEV, O_RDWR);
    if (fd < 0) {
        perror("Failed to open character device");
        err = 1;
        goto close_fib;
    }

    int fd_kt = open(KTIME_ENABLE, O_RDWR);
    if (fd_kt < 0) {
        perror("Failed to open sysfs");
        err = 1;
        fd = fd_kt;
        goto close_fib;
    }
    // Set ktimer
    write(fd_kt, "1", 2);
    close(fd_kt);

    int fd_fib = open(FIB_METHOD, O_RDWR);
    if (fd_fib < 0) {
        perror("Failed to open sysfs");
        err = 1;
        fd = fd_fib;
        goto close_fib;
    }
    // // Set fib method
    char *fib_index = "1";
    write(fd_fib, fib_index, 2);
    // close(fd_fib);

    bool bn_enable = false;

    FILE *fp = fopen("./plot/plot_input", "w");

    for (int i = 0; i <= offset; i++) {
        lseek(fd, i, SEEK_SET);
        clock_gettime(CLOCK_MONOTONIC, &t1);
        sz = read(fd, buf, 1);
        clock_gettime(CLOCK_MONOTONIC, &t2);
        ssize_t kt = write(fd, NULL, 0);
        long long utime = elapsed(&t1, &t2);
        if (bn_enable) {
            printf("Reading from " FIB_DEV
                   " at offset %d, returned the sequence "
                   "%s. utime %lld, ktime %ld\n",
                   i, buf, utime, kt);
        } else {
            printf("Reading from " FIB_DEV
                   " at offset %d, returned the sequence "
                   "%lld. utime %lld, ktime %ld\n",
                   i, sz, utime, kt);
        }

        fprintf(fp, "%d %lld %ld %lld\n", i, utime, kt, utime - kt);
    }

    close(fd);
    int fd1 = open(FIB_DEV, O_RDWR);
    if (fd1 < 0) {
        perror("Failed to open character device");
        err = 1;
        goto close_fib;
    }
    // Set fib method 2
    fib_index = "2";
    write(fd_fib, fib_index, 2);
    close(fd_fib);
    FILE *fp1 = fopen("./plot/plot_input1", "w");

    struct timespec t3, t4;

    for (int i = 0; i <= offset; i++) {
        lseek(fd1, i, SEEK_SET);
        clock_gettime(CLOCK_MONOTONIC, &t3);
        sz = read(fd1, buf, 1);
        clock_gettime(CLOCK_MONOTONIC, &t4);
        ssize_t kt = write(fd1, NULL, 0);
        long long utime = elapsed(&t3, &t4);
        if (bn_enable) {
            printf("Reading from " FIB_DEV
                   " at offset %d, returned the sequence "
                   "%s. utime %lld, ktime %ld\n",
                   i, buf, utime, kt);
        } else {
            printf("Reading from " FIB_DEV
                   " at offset %d, returned the sequence "
                   "%lld. utime %lld, ktime %ld\n",
                   i, sz, utime, kt);
        }

        fprintf(fp1, "%d %lld %ld %lld\n", i, utime, kt, utime - kt);
    }

close_fib:
    close(fd);
    return err;
}
