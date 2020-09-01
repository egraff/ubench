#ifndef UBENCH_INTRIN_ARM64_H
#define UBENCH_INTRIN_ARM64_H

#include "ubench_priv.h"
#include "ubench_compiler.h"

#include <stdint.h>


#if defined(_MSC_VER)
  #include <intrin.h>
#endif /* defined(_MSC_VER) */


#if defined(__GNUC__) && defined (__clang__)
  #include <arm_acle.h>
#endif /* defined(__GNUC__) && defined (__clang__) */


#if defined(_MSC_VER)
  /* https://developer.arm.com/docs/ddi0595/latest/aarch64-system-registers/cntvct_el0 */
  #define ARM64_CNTVCT_EL0 (ARM64_SYSREG(3, 3, 14, 0, 2))

  /* https://developer.arm.com/docs/ddi0595/latest/aarch64-system-registers/pmccntr_el0 */
  #define ARM64_PMCCNTR_EL0 (ARM64_SYSREG(3, 3, 9, 13, 0))
#endif /* defined(_MSC_VER) */


#if defined(__GNUC__)
  /* See ARM C Language Extensions 2.1
   * http://infocenter.arm.com/help/index.jsp?topic=/com.arm.doc.ihi0053d/index.html
   */
  #define _ARM64_BARRIER_SY 15
#endif /* defined(__GNUC__) */


#if defined(__GNUC__) && !defined (__clang__)
  #define __arm_rsr64(reg)                                            \
  ({                                                                  \
      int64_t val;                                                    \
      __asm__ __volatile__("mrs %0, " reg : "=r"(val));               \
      val;                                                            \
  })

  #define __dsb(type)                                                 \
  ({                                                                  \
      __asm__ __volatile__("dsb " STRINGIFY(type) : : : "memory");    \
  })

  #define __isb(type)                                                 \
  ({                                                                  \
      __asm__ __volatile__("isb " STRINGIFY(type) : : : "memory");    \
  })
#endif /* defined(__GNUC__) && !defined (__clang__) */


static FORCE_INLINE int64_t
arm64_read_cntvct_el0(void)
{
#if defined(_MSC_VER)
    return (int64_t)_ReadStatusReg(ARM64_CNTVCT_EL0);
#elif defined(__GNUC__)
    return (int64_t)__arm_rsr64("cntvct_el0");
#else
  #error "Unsupported compiler"
#endif
}


static FORCE_INLINE int64_t
arm64_read_pmccntr_el0(void)
{
#if defined(_MSC_VER)
    return (int64_t)_ReadStatusReg(ARM64_PMCCNTR_EL0);
#elif defined(__GNUC__)
    return (int64_t)__arm_rsr64("pmccntr_el0");
#else
  #error "Unsupported compiler"
#endif
}

#endif /* UBENCH_INTRIN_ARM64_H */
