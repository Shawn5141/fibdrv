#ifndef BIGN_H
#define BIGN_H
#include <linux/slab.h>
typedef struct _bn {
    unsigned long long lower, upper;
} bn;

// static inline void bn_init(bn *a){
//     a->lower = 0;
//     a->upper = 0;
//     return ;
// }

static inline void bn_init(bn *a, unsigned long long x)
{
    a->lower = x;
    a->upper = 0;
    return;
}

static inline void bn_add(bn *output, bn x, bn y)
{
    output->upper = x.upper + y.upper;

    if (y.lower > ~x.lower) {
        output->upper++;
        output->lower = x.lower + y.lower - ~0ULL;
    } else {
        output->lower = x.lower + y.lower;
    }
}

static inline void bn_sub(bn *output, bn x, bn y)
{
    long long tmp = ~0;
    output->upper = x.upper;
    if (x.lower < y.lower) {
        output->upper--;
        tmp -= y.lower;
    }
    output->lower = x.lower + tmp;
}

static inline char *bn_to_string(bn x)
{
    int lower_len = snprintf(NULL, 0, "%llu", x.lower);
    int upper_len = snprintf(NULL, 0, "%llu", x.upper);
    if (x.upper == 0) {
        char *str = kmalloc(lower_len + 1, GFP_KERNEL);
        snprintf(str, lower_len + 1, "%llu", x.lower);
        return str;
    }
    char *str = kmalloc(lower_len + upper_len + 1, GFP_KERNEL);
    snprintf(str, upper_len + lower_len + 1, "%llu%llu", x.upper, x.lower);
    return str;
}

#endif