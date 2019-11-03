#include "ubench_priv.h"
#include "ubench_arch_def.h"
#include "ubench_compiler.h"

#if defined(__i386__) || defined(__x86_64__)
  #include "ubench_intrin_x86.h"
#endif /* defined(__i386__) || defined(__x86_64__) */

#include <ubench/ubench_time.h>
#include <ubench/ubench.h>
#include <stdint.h>


#if defined(__i386__) || defined(__x86_64__)

/* Regarding the use of RDTSCP:
 * ----------------------------
 *
 * Intel SDM Volume 2B (https://software.intel.com/sites/default/files/managed/7c/f1/253667-sdm-vol-2b.pdf)
 * states that
 *
 *   [RDTSCP] is not a serializing instruction, but it does wait until all
 *   previous instructions have executed and all previous loads are globally
 *   visible. But it does not wait for previous stores to be globally visible,
 *   and subsequent instructions may begin execution before the read operation
 *   is performed
 *
 * and
 *
 *   If software requires RDTSCP to be executed prior to execution of any
 *   subsequent instruction (including any memory accesses), it can execute
 *   LFENCE immediately after RDTSCP.
 *
 * On the other hand, the AMD APM Volume 2 (https://www.amd.com/system/files/TechDocs/24593.pdf)
 * states somewhat vaguely that
 *
 *   The RDTSC instruction (unlike the RDTSCP instruction) is not serializing,
 *
 * but RDTSCP is not listed as one of the serializing non-privileged
 * instructions on AMD64 processors (CPUID, IRET, RSM, MFENCE). However, the
 * AMD white paper "Software Techniques For Managing Speculation on AMD
 * Processors" (https://developer.amd.com/wp-content/resources/Managing-Speculation-on-AMD-Processors.pdf)
 * clarifies that
 *
 *   [RDTSCP] is also dispatch serializing.
 *
 * This means that when using RDTSCP at the end of a measured region, we do
 * not need a preceding LFENCE, but we do need a subsequent LFENCE immediately
 * following the RDTSCP.
 */


static FORCE_INLINE void
cpuid_serialize(volatile uint32_t *aux)
{
    union cpu_info cpu_info;
    cpuid(0, &cpu_info);
    *aux = cpu_info.regs.eax;
}


static int
is_sse2_supported(void)
{
    union cpu_info cpu_info;

    cpuid(0x0, &cpu_info);
    if (cpu_info.regs.eax < 0x1)
    {
        return 0;
    }

    cpuid(0x1, &cpu_info);
    return !!(cpu_info.regs.edx & CPUID_BIT_SSE2);
}


static int
is_rdtscp_supported(void)
{
    union cpu_info cpu_info;

    cpuid(0x80000000, &cpu_info);
    if (cpu_info.regs.eax < 0x80000001)
    {
        return 0;
    }

    cpuid(0x80000001, &cpu_info);
    return !!(cpu_info.regs.edx & CPUID_BIT_RDTSCP);
}


static ubench_time_t
tsc_clock_start_with_sse2(struct ubench_clock *clock)
{
    ubench_time_t start_time;

    (void)clock;

    _mm_mfence();
    _mm_lfence();
    start_time = (ubench_time_t)__rdtsc();
    _mm_lfence();

    return start_time;
}


static ubench_time_t
tsc_clock_start_without_sse2(struct ubench_clock *clock)
{
    uint32_t aux;

    (void)clock;

    cpuid_serialize(&aux);
    return (ubench_time_t)__rdtsc();
}


static int64_t
tsc_clock_stop_with_rdtscp_with_sse2(
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
tsc_clock_stop_with_rdtscp_without_sse2(
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
tsc_clock_stop_without_rdtscp_with_sse2(
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


static int64_t
tsc_clock_stop_without_rdtscp_without_sse2(
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


static void
tsc_clock_warmup(struct ubench_clock *clock)
{
    clock->clock_stop(clock, clock->clock_start(clock));
    clock->clock_stop(clock, clock->clock_start(clock));
}


static void
tsc_clock_init(struct ubench_clock *clock)
{
    int has_sse2, has_rdtscp;

    has_sse2 = is_sse2_supported();
    has_rdtscp = is_rdtscp_supported();

    if (has_sse2)
    {
        clock->clock_start = tsc_clock_start_with_sse2;

        if (has_rdtscp)
        {
            clock->clock_stop = tsc_clock_stop_with_rdtscp_with_sse2;
        }
        else
        {
            clock->clock_stop = tsc_clock_stop_without_rdtscp_with_sse2;
        }
    }
    else
    {
        clock->clock_start = tsc_clock_start_without_sse2;

        if (has_rdtscp)
        {
            clock->clock_stop = tsc_clock_stop_with_rdtscp_without_sse2;
        }
        else
        {
            clock->clock_stop = tsc_clock_stop_without_rdtscp_without_sse2;
        }
    }

    clock->clock_warmup = tsc_clock_warmup;
}


const struct ubench_clock_cls ubench_arch_x86_tsc_clock_cls =
{
    .clock_init = tsc_clock_init,
};


const struct ubench_clock_cls * const ubench_cycle_clock_cls = &ubench_arch_x86_tsc_clock_cls;

#endif /* defined(__i386__) || defined(__x86_64__) */
