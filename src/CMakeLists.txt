set(ubench_src_files
    ubench.c
    ubench_empty_alg.c
    ubench_assert.c
    clocks/x86_with_sse2.c
    clocks/x86_without_sse2.c
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

    string(TOLOWER "${CMAKE_SYSTEM_PROCESSOR}" system_processor_lower)
    if("${system_processor_lower}" MATCHES "(x86|x64|amd64)")
        set_source_files_properties(clocks/x86_with_sse2.c PROPERTIES COMPILE_FLAGS -msse2)
    endif()
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
