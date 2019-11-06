#include "ubench_priv.h"
#include "ubench_arch_def.h"
#include "ubench_compiler.h"

#include <ubench/ubench_time.h>
#include <ubench/ubench.h>
#include <stdint.h>


#if defined(_MSC_VER)
  #include <intrin.h>
#endif /* defined(_MSC_VER) */


#if defined(__GNUC__)
  #if defined(__i386__) || defined(__x86_64__)
    #include <cpuid.h>
  #endif /* defined(__i386__) || defined(__x86_64__) */
#endif /* defined(__GNUC__) */


#if defined(__GNUC__) && defined(__INTEL_COMPILER)
  #if defined(__i386__) || defined(__x86_64__)
    #include <immintrin.h>
  #endif /* defined(__i386__) || defined(__x86_64__) */
#endif /* defined(__GNUC__) && defined(__INTEL_COMPILER) */


#if defined(__GNUC__) && !defined(__INTEL_COMPILER)
  #if defined(__i386__) || defined(__x86_64__)
    #include <x86intrin.h>
  #endif /* defined(__i386__) || defined(__x86_64__) */
#endif /* defined(__GNUC__) && !defined(__INTEL_COMPILER) */


#if defined(__GNUC__) && !defined(__clang__)
  #if !GNUC_PREREQ(4, 5)
static FORCE_INLINE int64_t
__rdtsc()
{
    uint32_t eax, edx;
    uint64_t res;

    __asm__ __volatile__("rdtsc" : "=a" (eax), "=d" (edx));
    res = ((uint64_t)edx << 32) | eax;
    return *((int64_t *)&res);
}


static FORCE_INLINE int64_t
__rdtscp(uint32_t *aux)
{
    uint32_t eax, ecx, edx;
    uint64_t res;

    __asm__ __volatile__("rdtscp" : "=a" (eax), "=d" (edx), "=c" (ecx));
    res = ((uint64_t)edx << 32) | eax;
    *aux = ecx;
    return *((int64_t *)&res);
}
  #endif /* !GNUC_PREREQ(4, 5) */
#endif /* defined(__GNUC__) && !defined(__clang__) */


#if defined(__i386__) || defined(__x86_64__)

static FORCE_INLINE void
cpuid(uint32_t leaf, uint32_t cpu_info[4])
{
#if defined(_MSC_VER)
    __cpuid((int *)cpu_info, leaf);
#elif defined(__GNUC__)
    __cpuid(leaf, cpu_info[0], cpu_info[1], cpu_info[2], cpu_info[3]);
#endif
}


static inline void
cpuid_serialize(volatile uint32_t *aux)
{
    uint32_t cpu_info[4];
    cpuid(0, cpu_info);
    *aux = cpu_info[0];
}


static void
tsc_clock_warmup(struct ubench_clock *clock)
{
    uint32_t ui, aux;

    (void)clock;

    cpuid_serialize(&aux);
    (void)__rdtsc();

    (void)__rdtscp(&ui);
    cpuid_serialize(&aux);

    cpuid_serialize(&aux);
    (void)__rdtsc();

    (void)__rdtscp(&ui);
    cpuid_serialize(&aux);
}


static ubench_time_t
tsc_clock_start(struct ubench_clock *clock)
{
    uint32_t aux;

    (void)clock;

    cpuid_serialize(&aux);
    return (ubench_time_t)__rdtsc();
}


static int64_t
tsc_clock_stop(struct ubench_clock *clock, ubench_time_t start_time)
{
    ubench_time_t end_time;
    uint32_t      ui, aux;

    (void)clock;

    end_time = (ubench_time_t)__rdtscp(&ui);
    cpuid_serialize(&aux);

    return ubench_time_diff(end_time, start_time);
}


static void
tsc_clock_init(struct ubench_clock *clock)
{
    clock->clock_warmup = tsc_clock_warmup;
    clock->clock_start = tsc_clock_start;
    clock->clock_stop = tsc_clock_stop;
}


const struct ubench_clock_cls x86_tsc_clock_cls =
{
    .clock_init = tsc_clock_init,
};

#endif /* defined(__i386__) || defined(__x86_64__) */
