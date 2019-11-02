#include "ubench_priv.h"
#include "ubench_compiler.h"

#ifdef NDEBUG
#undef NDEBUG
#endif

#include <assert.h>


#if defined(_MSC_VER) || defined(__MINGW__)

#include <wchar.h>

void
ubench_assert_fn(const wchar_t *expr, const wchar_t *file, unsigned int line)
{
    _wassert(expr, file, line);
}

#elif defined(__GNUC__) /* defined(_MSC_VER) || defined(__MINGW__) */

void
ubench_assert_fn(
    const char *expr,
    const char *file,
    unsigned int line,
    const char *func
)
{
  #if defined(__linux__)
    __assert_fail(expr, file, line, func);
  #elif defined(__APPLE__) /* defined(__linux__) */
    __assert_rtn(func, file, line, expr);
  #elif defined(__CYGWIN__) /* defined(__APPLE__) */
    __assert_func(file, line, func, expr);
  #else /* defined(__CYGWIN__) */
    #error "Unsupported platform"
  #endif /* !defined(__CYGWIN__) */
}

#else /* defined(__GNUC__) */
  #error "Unsupported compiler"
#endif /* !defined(__GNUC__) */
