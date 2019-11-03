#ifndef UBENCH_COMPILER_H
#define UBENCH_COMPILER_H

#if (defined(__MINGW32__) || defined(__MINGW64__)) && !defined(__MINGW__)
  #define __MINGW__ 1
#endif


#if defined(__STDC_VERSION__) && (__STDC_VERSION__ >= 201102L) && !defined(__STDC_NO_THREADS__)
  #include <threads.h>
#endif


#if !defined(thread_local)
  #if defined(_MSC_VER)
    #define thread_local __declspec(thread)
  #elif defined(__GNUC__)
    #define thread_local __thread
  #endif
#endif


#if !defined(thread_local)
  #error "No thread_local support"
#endif


#define FORCE_INLINE inline


#if defined(_MSC_VER)
  #undef FORCE_INLINE
  #define FORCE_INLINE __forceinline
#endif /* defined(_MSC_VER) */


#if defined(__GNUC__)
  #undef FORCE_INLINE
  #define FORCE_INLINE inline __attribute__((always_inline))

  #define GNUC_PREREQ(maj, min) (                                           \
    (__GNUC__ > maj) || ((__GNUC__ == maj) && (__GNUC_MINOR__ >= min))      \
  )
#endif /* defined(__GNUC__) */

#endif /* UBENCH_COMPILER_H */
