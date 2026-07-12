cmake_minimum_required(VERSION 3.25)

project(Myc
    VERSION 0.1.0
    DESCRIPTION "Myc systems programming language toolchain"
    LANGUAGES CXX
)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
    set(CMAKE_BUILD_TYPE "Debug" CACHE STRING "Build type (Debug or Release)" FORCE)
    set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS Debug Release RelWithDebInfo MinSizeRel)
endif()

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")
include(MycOptions)
include(CompilerWarnings)

if(MYC_ENABLE_LTO AND CMAKE_BUILD_TYPE STREQUAL "Release")
    include(CheckIPOSupported)
    check_ipo_supported(RESULT ipo_supported OUTPUT ipo_error)
    if(ipo_supported)
        set(CMAKE_INTERPROCEDURAL_OPTIMIZATION TRUE)
    else()
        message(WARNING "LTO not supported: ${ipo_error}")
    endif()
endif()

# Shared infrastructure used across the toolchain.
add_subdirectory(shared)

# Compiler pipeline modules.
add_subdirectory(compiler)

# Language runtime and standard library.
add_subdirectory(runtime)
add_subdirectory(stdlib)

# Data structures and algorithms library.
add_subdirectory(dsa)

# Optimization passes (future LLVM integration point).
add_subdirectory(optimizer)

# Code generation backends.
add_subdirectory(backend)
add_subdirectory(frontend)

# Tooling ecosystem.
add_subdirectory(package-manager)
add_subdirectory(language-server)
add_subdirectory(formatter)
add_subdirectory(linter)
add_subdirectory(debugger)
add_subdirectory(ide)

# Tests, benchmarks, and examples.
if(MYC_BUILD_TESTS)
    enable_testing()
    add_subdirectory(tests)
endif()

if(MYC_BUILD_BENCHMARKS)
    add_subdirectory(benchmarks)
endif()

add_subdirectory(examples)

# Compiler driver executable.
add_executable(myc
    compiler/driver/src/main.cpp
)

target_link_libraries(myc PRIVATE myc_driver)
myc_set_target_standard(myc)
myc_set_warnings(myc)

set_target_properties(myc PROPERTIES
    OUTPUT_NAME "myc"
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
)

install(TARGETS myc RUNTIME DESTINATION bin)

message(STATUS "Myc ${PROJECT_VERSION} — ${CMAKE_BUILD_TYPE} build")
message(STATUS "C++ standard: C++${MYC_CXX_STANDARD}")
