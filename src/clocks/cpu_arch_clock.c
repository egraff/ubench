#include "ubench_priv.h"
#include "ubench_arch_def.h"
#include "ubench_compiler.h"

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
tsc_clock_init(void **clock_data)
{
    uint32_t ui, aux;

    (void)clock_data;

    cpuid_serialize(&aux);
    (void)__rdtsc();

    (void)__rdtscp(&ui);
    cpuid_serialize(&aux);

    cpuid_serialize(&aux);
    (void)__rdtsc();

    (void)__rdtscp(&ui);
    cpuid_serialize(&aux);
}


static void
tsc_clock_warmup(const void *clock_data)
{
    uint32_t ui, aux;

    (void)clock_data;

    cpuid_serialize(&aux);
    (void)__rdtsc();

    (void)__rdtscp(&ui);
    cpuid_serialize(&aux);

    cpuid_serialize(&aux);
    (void)__rdtsc();

    (void)__rdtscp(&ui);
    cpuid_serialize(&aux);
}


static int64_t
tsc_clock_start(void *clock_data)
{
    uint32_t aux;

    (void)clock_data;

    cpuid_serialize(&aux);
    return (int64_t)__rdtsc();
}


static int64_t
tsc_clock_stop(void *clock_data, int64_t start_val)
{
    uint32_t  ui, aux;
    int64_t   val;

    (void)clock_data;

    val = (int64_t)__rdtscp(&ui);
    cpuid_serialize(&aux);

    return val - start_val;
}


const struct ubench_clock x86_tsc_clock =
{
    .clock_init = tsc_clock_init,
    .clock_warmup = tsc_clock_warmup,
    .clock_start = tsc_clock_start,
    .clock_stop = tsc_clock_stop,
};

#endif /* defined(__i386__) || defined(__x86_64__) */
