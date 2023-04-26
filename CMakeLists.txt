cmake_minimum_required (VERSION 3.0)

if(APPLE)
  # Build a Universal binary on macOS
  set(CMAKE_OSX_ARCHITECTURES "arm64;x86_64" CACHE STRING "" FORCE)
endif()

project(llama-chat VERSION 0.1.0)

set(CMAKE_EXPORT_COMPILE_COMMANDS "on")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_INSTALL_RPATH "${CMAKE_INSTALL_PREFIX}/lib")


# options
option(LLAMA_AVX                    "llama: enable AVX"                                     ON)
option(LLAMA_AVX2                   "llama: enable AVX2"                                    OFF)
option(LLAMA_AVX512                 "llama: enable AVX512"                                  OFF)
option(LLAMA_AVX512_VBMI            "llama: enable AVX512-VBMI"                             OFF)
option(LLAMA_AVX512_VNNI            "llama: enable AVX512-VNNI"                             OFF)
option(LLAMA_FMA                    "llama: enable FMA"                                     OFF)


# sanitizers
#set(LLAMA_BUILD_EXAMPLES ON CACHE BOOL "llama: build examples" FORCE)
set(BUILD_SHARED_LIBS ON FORCE)


if (GGML_SANITIZE_THREAD)
    set(CMAKE_C_FLAGS   "${CMAKE_C_FLAGS}   -fsanitize=thread")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=thread")
endif()

if (GGML_SANITIZE_ADDRESS)
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS}     -fsanitize=address -fno-omit-frame-pointer")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=address -fno-omit-frame-pointer")
endif()

if (GGML_SANITIZE_UNDEFINED)
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS}     -fsanitize=undefined")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=undefined")
endif()

#set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -ffast-math")
#set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -march=native")
#set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -mcpu=native")

# dependencies

set(CMAKE_C_STANDARD   17)
set(CMAKE_CXX_STANDARD 20)

find_package(Threads REQUIRED)

# main

if (NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
    set(CMAKE_BUILD_TYPE Release CACHE STRING "Build type" FORCE)
    set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS "Debug" "Release" "RelWithDebInfo")
endif ()

add_subdirectory(llama.cpp)
add_subdirectory(src)