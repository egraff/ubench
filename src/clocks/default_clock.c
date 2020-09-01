#include "ubench_arch_def.h"


#if defined(__i386__) || defined(__x86_64__)

extern const struct ubench_clock_cls ubench_arch_x86_tsc_clock_cls;
const struct ubench_clock_cls * const ubench_default_clock_cls = &ubench_arch_x86_tsc_clock_cls;

#elif defined(__arm64__) /* defined(__i386__) || defined(__x86_64__) */

extern const struct ubench_clock_cls ubench_arch_arm64_cntvct_clock_cls;
const struct ubench_clock_cls * const ubench_default_clock_cls = &ubench_arch_arm64_cntvct_clock_cls;

#else /* defined(__arm64__) */
  #error "Unsupported architecture"
#endif
