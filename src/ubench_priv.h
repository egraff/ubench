#ifndef UBENCH_PRIV_H
#define UBENCH_PRIV_H

#define STR_TO_WIDE_(s) L ## s
#define STR_TO_WIDE(s) STR_TO_WIDE_(s)


#define STRINGIFY(x) #x
#define STR(x) STRINGIFY(x)
#define WSTR(x) STR_TO_WIDE(STRINGIFY(x))


#if defined(_MSC_VER)
  #include <wchar.h>

  #define ubench_assert(expr)                                               \
  (                                                                         \
      (!(expr))                                                             \
      ? ubench_assert_fn(WSTR(expr), STR_TO_WIDE(__FILE__), __LINE__)       \
      : (void)0                                                             \
  )


extern void
ubench_assert_fn(const wchar_t *expr, const wchar_t *file, unsigned int line);

#elif defined(__GNUC__) /* defined(_MSC_VER) */
  #define ubench_assert(expr)                                               \
  (                                                                         \
      (!(expr))                                                             \
      ? ubench_assert_fn(STR(expr), __FILE__, __LINE__, __func__)           \
      : (void)0                                                             \
  )


extern void
ubench_assert_fn(
    const char *expr,
    const char *file,
    unsigned int line,
    const char *func
);

#else
  #error "Unsupported compiler"
#endif /* defined(__GNUC__) */

#endif /* UBENCH_PRIV_H */
