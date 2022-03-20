#ifndef BIGN_H
#define BIGN_H


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
    if (y.lower > ~x.lower)
        output->upper++;
    output->lower = x.lower + y.lower;
}

static inline void bn_subtract(bn *output, bn x, bn y)
{
    long long tmp = ~0;
    output->upper = x.upper;
    if (x.lower < y.lower) {
        output->upper--;
        tmp -= y.lower;
    }
    output->lower = x.lower + tmp;
}

#endif