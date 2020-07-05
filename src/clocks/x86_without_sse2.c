#include "ubench_arch_def.h"
#include "ubench_compiler.h"

#if defined(__i386__) || defined(__x86_64__)

#include "ubench_intrin_x86.h"

#include <ubench/ubench_time.h>
#include <ubench/ubench.h>
#include <stdint.h>


static FORCE_INLINE void
cpuid_serialize(volatile uint32_t *aux)
{
    union cpu_info cpu_info;
    cpuid(0, &cpu_info);
    *aux = cpu_info.regs.eax;
}


static ubench_time_t
x86_without_sse2__tsc_clock_start(struct ubench_clock *clock)
{
    uint32_t aux;

    (void)clock;

    cpuid_serialize(&aux);
    return (ubench_time_t)__rdtsc();
}


static int64_t
x86_without_sse2__tsc_clock_stop_with_rdtscp(
    struct ubench_clock *clock,
    ubench_time_t start_time
)
{
    ubench_time_t end_time;
    uint32_t      ui, aux;

    (void)clock;

    end_time = (ubench_time_t)__rdtscp(&ui);
    cpuid_serialize(&aux);

    return ubench_time_diff(end_time, start_time);
}


static int64_t
x86_without_sse2__tsc_clock_stop_without_rdtscp(
    struct ubench_clock *clock,
    ubench_time_t start_time
)
{
    ubench_time_t end_time;
    uint32_t      aux;

    (void)clock;

    cpuid_serialize(&aux);
    end_time = (ubench_time_t)__rdtsc();
    cpuid_serialize(&aux);

    return ubench_time_diff(end_time, start_time);
}


void
x86_without_sse2__tsc_clock_init(struct ubench_clock *clock, int has_rdtscp)
{
    clock->clock_start = x86_without_sse2__tsc_clock_start;

    if (has_rdtscp)
    {
        clock->clock_stop = x86_without_sse2__tsc_clock_stop_with_rdtscp;
    }
    else
    {
        clock->clock_stop = x86_without_sse2__tsc_clock_stop_without_rdtscp;
    }
}

#endif /* defined(__i386__) || defined(__x86_64__) */
