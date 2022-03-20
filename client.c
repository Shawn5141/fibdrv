#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <time.h>
#include <unistd.h>

#define FIB_DEV "/dev/fibonacci"
#define KTIME_ENABLE "/sys/class/fibonacci/fibonacci/ktime_measure"
#define FIB_METHOD "/sys/class/fibonacci/fibonacci/fib_method"



static inline long long elapsed(struct timespec *t1, struct timespec *t2)
{
    return (long long) (t2->tv_sec - t1->tv_sec) * 1e9 +
           (long long) (t2->tv_nsec - t1->tv_nsec);
}

int main()
{
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
    // Set fib method
    write(fd_fib, "0", 2);
    close(fd_fib);

    FILE *fp = fopen("./plot/plot_input", "w");

    for (int i = 0; i <= offset; i++) {
        lseek(fd, i, SEEK_SET);
        clock_gettime(CLOCK_MONOTONIC, &t1);
        sz = read(fd, buf, 1);
        clock_gettime(CLOCK_MONOTONIC, &t2);
        ssize_t kt = write(fd, NULL, 0);
        long long utime = elapsed(&t1, &t2);
        printf("Reading from " FIB_DEV
               " at offset %d, returned the sequence "
               "%lld. utime %lld, ktime %ld\n",
               i, sz, utime, kt);
        fprintf(fp, "%d %lld %ld %lld\n", i, utime, kt, utime - kt);
    }

close_fib:
    close(fd);
    return err;
}
