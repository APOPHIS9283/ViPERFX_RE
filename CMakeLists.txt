cmake_minimum_required(VERSION 3.16.3)
project("ViPER4Android")

set(VERSION_CODE "00000000" CACHE STRING "Build version code")
set(VERSION_NAME "0.0.0" CACHE STRING "Build version name")

add_compile_definitions(VERSION_CODE=${VERSION_CODE})
add_compile_definitions(VERSION_NAME="${VERSION_NAME}")

set(V4A_RE_COMMON_FLAGS
    -O3 -flto -DNDEBUG
    -ffunction-sections -fdata-sections
    -fno-exceptions -fno-rtti
    -fno-unwind-tables -fno-asynchronous-unwind-tables
    -fvisibility=hidden -fvisibility-inlines-hidden)

add_subdirectory(ViPERDSP)

target_compile_options(ViPERDSP PRIVATE ${V4A_RE_COMMON_FLAGS})

add_library(v4a_re SHARED
    src/ViPER4Android.cpp
    src/ViperContext.cpp
)

target_include_directories(v4a_re PRIVATE
    src
    src/include
)

target_link_libraries(v4a_re ViPERDSP log)

target_compile_options(v4a_re PRIVATE
    ${V4A_RE_COMMON_FLAGS}
    -Wall -Wno-unused-parameter -Wno-unused-variable -Wno-unused-function)

target_link_options(v4a_re PRIVATE
    -flto
    -Wl,--gc-sections
    -Wl,--strip-all
    -Wl,--exclude-libs,ALL
    -fvisibility=hidden)
