#ifndef UBENCH_INTRIN_X86_H
#define UBENCH_INTRIN_X86_H

#include "ubench_compiler.h"

#include <stdint.h>


#if defined(_MSC_VER)
  #include <intrin.h>
#endif /* defined(_MSC_VER) */


#if defined(__GNUC__)
  #include <cpuid.h>
#endif /* defined(__GNUC__) */


#if defined(__GNUC__) && defined(__INTEL_COMPILER)
  #include <immintrin.h>
#endif /* defined(__GNUC__) && defined(__INTEL_COMPILER) */


#if defined(__GNUC__) && !defined(__INTEL_COMPILER)
  #include <x86intrin.h>
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


#define CPUID_BIT_SSE2 (1 << 26)
#define CPUID_BIT_RDTSCP (1 << 27)


union cpu_info
{
    int32_t cpu_info_l[4];
    uint32_t cpu_info_u[4];
    struct
    {
        uint32_t eax;
        uint32_t ebx;
        uint32_t ecx;
        uint32_t edx;
    } regs;
};


static FORCE_INLINE void
cpuid(uint32_t leaf, union cpu_info *cpu_info)
{
#if defined(_MSC_VER)
    __cpuid(cpu_info->cpu_info_l, leaf);
#elif defined(__GNUC__)
    __cpuid(
        leaf,
        cpu_info->regs.eax,
        cpu_info->regs.ebx,
        cpu_info->regs.ecx,
        cpu_info->regs.edx
    );
#else
  #error "Unsupported compiler"
#endif
}

#endif /* UBENCH_INTRIN_X86_H */
