#include "ubench_arch_def.h"


#if defined(__i386__) || defined(__x86_64__)

extern const struct ubench_clock_cls ubench_arch_x86_tsc_clock_cls;
const struct ubench_clock_cls * const ubench_default_clock_cls = &ubench_arch_x86_tsc_clock_cls;

#else /* defined(__i386__) || defined(__x86_64__) */
  #error "Unsupported architecture"
#endif
