#ifndef UBENCH_TIME_H
#define UBENCH_TIME_H

#include <ubench/ubench.h>
#include <stdint.h>


static inline int64_t
ubench_time_diff(ubench_time_t end_time, ubench_time_t start_time)
{
    if (end_time >= start_time)
    {
        return (int64_t)((ubench_time_t)(end_time - start_time));
    }
    else
    {
        return -(int64_t)((ubench_time_t)(start_time - end_time));
    }
}

#endif /* UBENCH_TIME_H */
