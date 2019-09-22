set(SRC_FILES
    ubench.c
    ubench_empty_alg.c
    ubench_assert.c
    clocks/cpu_arch_clock.c
)

add_library(ubench STATIC ${SRC_FILES})

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
        PUBLIC_HEADER "${PROJECT_SOURCE_DIR}/include/ubench/ubench.h"
)

INSTALL(TARGETS ubench
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib/static
    PUBLIC_HEADER DESTINATION include/ubench
)
