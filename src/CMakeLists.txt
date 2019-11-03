set(ubench_src_files
    ubench.c
    ubench_empty_alg.c
    ubench_assert.c
    clocks/cpu_arch_clock.c
    clocks/default_clock.c
)

set(ubench_public_header_files
    "${PROJECT_SOURCE_DIR}/include/ubench/ubench.h"
    "${PROJECT_SOURCE_DIR}/include/ubench/ubench_time.h"
)

add_library(ubench STATIC ${ubench_src_files})

if(MSVC)
    target_compile_options(ubench PUBLIC /TC /W4 /WX /FA)
else()
    target_compile_options(ubench PUBLIC -std=c99 -Wall -Werror -Wextra)
endif()

target_include_directories(ubench
    PUBLIC ${PROJECT_SOURCE_DIR}/include
    PRIVATE .
)

set_target_properties(ubench
    PROPERTIES
        PUBLIC_HEADER "${ubench_public_header_files}"
)

INSTALL(TARGETS ubench
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib/static
    PUBLIC_HEADER DESTINATION include/ubench
)
