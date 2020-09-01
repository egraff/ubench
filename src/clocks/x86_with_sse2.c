#include "ubench_arch_def.h"
#include "ubench_compiler.h"

#if defined(__i386__) || defined(__x86_64__)

#include "ubench_intrin_x86.h"

#include <ubench/ubench_time.h>
#include <ubench/ubench.h>
#include <stdint.h>


static ubench_time_t
x86_with_sse2__tsc_clock_start(struct ubench_clock *clock)
{
    ubench_time_t start_time;

    (void)clock;

    _mm_mfence();
    _mm_lfence();
    start_time = (ubench_time_t)__rdtsc();
    _mm_lfence();

    return start_time;
}


static int64_t
x86_with_sse2__tsc_clock_stop_with_rdtscp(
    struct ubench_clock *clock,
    ubench_time_t start_time
)
{
    ubench_time_t end_time;
    uint32_t      ui;

    (void)clock;

    end_time = (ubench_time_t)__rdtscp(&ui);
    _mm_lfence();

    return ubench_time_diff(end_time, start_time);
}


static int64_t
x86_with_sse2__tsc_clock_stop_without_rdtscp(
    struct ubench_clock *clock,
    ubench_time_t start_time
)
{
    ubench_time_t end_time;

    (void)clock;

    _mm_lfence();
    end_time = (ubench_time_t)__rdtsc();
    _mm_lfence();

    return ubench_time_diff(end_time, start_time);
}


void
x86_with_sse2__tsc_clock_init(struct ubench_clock *clock, int has_rdtscp)
{
    clock->clock_start = x86_with_sse2__tsc_clock_start;

    if (has_rdtscp)
    {
        clock->clock_stop = x86_with_sse2__tsc_clock_stop_with_rdtscp;
    }
    else
    {
        clock->clock_stop = x86_with_sse2__tsc_clock_stop_without_rdtscp;
    }
}

#else /* defined(__i386__) || defined(__x86_64__) */

typedef void prevent_empty_translation_unit;

#endif
