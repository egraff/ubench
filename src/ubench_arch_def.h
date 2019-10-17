#ifndef UBENCH_ARCH_DEF_H
#define UBENCH_ARCH_DEF_H

#if (defined(_M_IX86) || defined(__i386)) && !defined(__i386__)
  #define __i386__ 1
#endif


#if (defined(_M_X64) || defined(_M_AMD64) || defined(__x86_64) || defined(__amd64__) || defined(__amd64)) && !defined(__x86_64__)
  #define __x86_64__ 1
#endif


#if (defined(_M_ARM) || defined(_ARM) || defined(__arm)) && !defined(__arm__)
  #define __arm__ 1
#endif


#if (defined(_M_ARM64) || defined(__aarch64__) || defined(__arm64)) && !defined(__arm64__)
  #define __arm64__ 1
#endif

#endif /* UBENCH_ARCH_DEF_H */
