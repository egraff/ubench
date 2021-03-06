set(CMAKE_SYSTEM_NAME Linux)
set(CMAKE_SYSTEM_PROCESSOR arm64)

set(cross_suffix "")
if(DEFINED ENV{CROSS_SUFFIX})
  set(cross_suffix $ENV{CROSS_SUFFIX})
endif()

set(triple aarch64-linux-gnu)
if(DEFINED ENV{CLANG_TRIPLE})
  set(triple $ENV{CLANG_TRIPLE})
endif()

if(NOT DEFINED(CMAKE_C_COMPILER))
  set(CMAKE_C_COMPILER clang${cross_suffix})
endif()

if(NOT DEFINED(CMAKE_CXX_COMPILER))
  set(CMAKE_CXX_COMPILER clang++${cross_suffix})
endif()
